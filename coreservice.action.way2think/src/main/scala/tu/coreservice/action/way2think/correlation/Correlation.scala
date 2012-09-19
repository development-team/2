package tu.coreservice.action.way2think.correlation

import tu.model.knowledge.annotator.AnnotatedNarrative
import tu.model.knowledge.domain.{ConceptLink, Concept, ConceptNetwork}
import tu.coreservice.action.way2think.SimulationReformulationAbstract
import tu.model.knowledge.Constant
import tu.exception.UnexpectedException

/**
 * @author max talanov
 *         date 2012-09-10
 *         time: 11:15 PM
 */
class Correlation extends SimulationReformulationAbstract {

  /**
   * Process clarification over simulationResult, creating mapping from simulationResult to domainModel
   * @param clarification AnnotatedNarrative to map simulationResult to domainModel
   * @param simulationResult clarified
   * @param domainModel model to be mapped to
   * @return Option of Triple of shortestMaps, domainConcepts, notUnderstood concepts from clarification response.
   */
  def apply(clarification: AnnotatedNarrative,
            simulationResult: ConceptNetwork,
            domainModel: ConceptNetwork): Option[Triple[List[Concept], List[Concept], List[Concept]]] = {
    val notKnown: List[Concept] = filterConceptListNegative(simulationResult.nodes, domainModel)
    if (notKnown.size > 0) {
      val processed = processNotKnown(notKnown, clarification, domainModel)
      Some(processed._1, processed._2, processed._3)
    } else {
      None
    }
  }

  /**
   * Process clarification (simple training), creating mapping from clarification to domainModel
   * @param clarification AnnotatedNarrative to map simulationResult to domainModel
   * @param domainModel model to be mapped to
   * @return Option of Triple of shortestMaps, domainConcepts, notUnderstood concepts from clarification response.
   */
  def apply(clarification: AnnotatedNarrative,
            domainModel: ConceptNetwork): Option[Triple[List[Concept], List[Concept], List[Concept]]] = {
      val processed = processClarification(clarification, domainModel)
      Some(processed._1, processed._2, processed._3)
  }

  /**
   * Creates mapping of notKnown List[Concept] to targetModel via mappingNarrative concepts, creating new concept List.
   * @param mappingNarrative AnnotatedNarrative to be used for mapping.
   * @param targetModel the List of Concept-s to be mapped to.
   * @return Triple of shortestMaps, domainConcepts, notUnderstood concepts from clarification response.
   */
  def processClarification(mappingNarrative: AnnotatedNarrative,
                      targetModel: ConceptNetwork): Triple[List[Concept], List[Concept], List[Concept]] = {
    val clarifiedConcepts = mappingNarrative.conceptNetwork.nodes
    val clarifiedTargetConcepts = clarifiedConcepts.filter {
      c: Concept => {
        findInTarget(c, targetModel).size < 1
      }
    }
    val shortestMaps: List[List[Concept]] = clarifiedTargetConcepts.map {
      c: Concept => {
        findMapToTarget(c, targetModel, List[Concept]())
      }
    }
    val notUnderstood = this.checkShortestMaps(shortestMaps, targetModel)
    val domainConcepts = createDomainConcepts(shortestMaps.flatten)
    (shortestMaps.flatten, domainConcepts, notUnderstood)
  }

  /**
   * Creates mapping of notKnown List[Concept] to targetModel via mappingNarrative concepts, creating new concept List.
   * @param notKnown List of concepts to be mapped.
   * @param mappingNarrative AnnotatedNarrative to be used for mapping.
   * @param targetModel the List of Concept-s to be mapped to.
   * @return Triple of shortestMaps, domainConcepts, notUnderstood concepts from clarification response.
   */
  def processNotKnown(notKnown: List[Concept],
                      mappingNarrative: AnnotatedNarrative,
                      targetModel: ConceptNetwork): Triple[List[Concept], List[Concept], List[Concept]] = {
    val clarifiedConcepts = filterConceptList(notKnown, mappingNarrative.conceptNetwork)
    val clarifiedTargetConcepts = clarifiedConcepts.filter {
      c: Concept => {
        findInTarget(c, targetModel).size < 1
      }
    }
    val shortestMaps: List[List[Concept]] = clarifiedTargetConcepts.map {
      c: Concept => {
        findMapToTarget(c, targetModel, List[Concept]())
      }
    }
    val notUnderstood = this.checkShortestMaps(shortestMaps, targetModel)
    val domainConcepts = createDomainConcepts(shortestMaps.flatten)
    (shortestMaps.flatten, domainConcepts, notUnderstood)
  }

  /**
   * Checks if all shortest maps tail concepts are in targetModel.
   * @param shortestMaps = maps to check
   * @return List of tail concepts not found in targetModel.
   */
  def checkShortestMaps(shortestMaps: List[List[Concept]], targetModel: ConceptNetwork): List[Concept] = {
    val notUnderstood = shortestMaps.filter {
      c: List[Concept] => {
        targetModel.getNodeByName(c.last.uri.name).size > 0
      }
    }
    notUnderstood.map {
      c: List[Concept] => {
        c.last
      }
    }
  }

  def createDomainConcepts(mappingConceptsInstances: List[Concept]): List[Concept] = {
    mappingConceptsInstances.map {
      c: Concept => {
        if (c.uri.name.contains(Constant.UID_INSTANCE_DELIMITER)) {
          val parentName = c.uri.name.substring(c.uri.name.indexOf(Constant.UID_INSTANCE_DELIMITER))
          val parentConcept = Concept(parentName)
          c.generalisations = c.generalisations + (parentConcept.uri -> parentConcept)
          parentConcept
        } else {
          throw new UnexpectedException("$Can_not_create_parent_not_from_instance")
        }
      }
    }
  }

  def findInTarget(mappingConcept: Concept, targetModel: ConceptNetwork): List[Concept] = {
    targetModel.getNodeByName(mappingConcept.uri.name)
  }

  def findMapToTarget(mappingConcept: Concept, targetModel: ConceptNetwork, processedConcepts: List[Concept]): List[Concept] = {
    if (findInTarget(mappingConcept, targetModel).size > 0) {
      List(mappingConcept)
    } else {
      if (mappingConcept.links.size > 0) {
        val filteredLinks = filterLinks(mappingConcept.links, mappingConcept, processedConcepts)
        val intermediateConcepts = filterLinksConcepts(filteredLinks, mappingConcept)
        val res: List[List[Concept]] = intermediateConcepts.map {
          c: Concept => {
            findMapToTarget(c, targetModel, processedConcepts ::: List(mappingConcept))
          }
        }
        val foundMappings: List[List[Concept]] = res.filter {
          lC: List[Concept] => lC.size > 0
        }
        val shortestMapping: List[Concept] = foundMappings.reduceLeft((s1, s2) =>
          if (s2.size > s1.size) s1
          else s2
        )
        shortestMapping
      } else {
        List[Concept]()
      }
    }
  }

  def filterLinks(links: List[ConceptLink], currentConcept: Concept, processedLinks: List[Concept]): List[ConceptLink] = {
    links.filter {
      link: ConceptLink => {
        if (link.source == currentConcept) {
          val destination = link.destination
          !processedLinks.contains(destination)
        } else {
          val source = link.source
          !processedLinks.contains(source)
        }
      }
    }
  }

  def filterLinksConcepts(links: List[ConceptLink], currentConcept: Concept): List[Concept] = {
    links.map {
      link: ConceptLink => {
        if (link.source == currentConcept) {
          link.destination

        } else {
          link.source
        }
      }
    }
  }
}
