package tu.coreservice.action.critic.manager

import tu.model.knowledge.{Constant, Resource, Probability, KnowledgeURI}
import tu.model.knowledge.communication.{ContextHelper, ShortTermMemory}
import tu.model.knowledge.helper.URIGenerator
import tu.coreservice.action.critic.{CriticLink, Critic}

/**
 * @author alex toschev
 *         date 2014-09-08
 *         time: 10:09 PM
 */
class DoNotUnderstandManager(_exclude: List[CriticLink], _include: List[CriticLink], _uri: KnowledgeURI, _probability: Probability = new Probability())
  extends Critic(_exclude, _include, _uri, _probability) {


  def this() = this(List[CriticLink](), List[CriticLink](), URIGenerator.generateURI("DoNotUnderstandManager"))

  def start() = false

  def stop() = false

  /**
   * Starts DoNotUnderstand that invokes Cry4Help.
   * @param inputContext ShortTermMemory of all inbound parameters
   * @return output ShortTermMemory.
   */
  override def apply(inputContext: ShortTermMemory): ShortTermMemory = {
    inputContext.lastError match {
      case Some(error: Error) => {
        val d = new DoNotUnderstand()
        ContextHelper(List[Resource](), d(error), this.getClass.getName + " result")
      }
      case None => {
        if (inputContext.notUnderstoodConcepts.size > 0) {
          val d = new DoNotUnderstand()
          ContextHelper.createReflectiveContext(d(inputContext.notUnderstoodConcepts), this.getClass.getName + " result")
        } else if (inputContext.notUnderstoodPhrases.size > 0) {
          val d = new DoNotUnderstand()
          ContextHelper.createReflectiveContext(d(), this.getClass.getName + " result")
        } else if (inputContext.resultToReport.size > 0) {
          val d = new DoNotUnderstand()
          ContextHelper.createReflectiveContext(d(inputContext.resultToReport), this.getClass.getName + " " + Constant.RESULT_TO_REPORT)
        } else if (inputContext.solutionsToReport.size > 0) {
          val d = new DoNotUnderstand()
          ContextHelper.createReflectiveContext(d(inputContext.resultToReport), this.getClass.getName + " " + Constant.FOUND_SOLUTIONS)
        }else {
          ContextHelper(List[Resource](), this.getClass.getName + " result")
        }
      }
    }
  }
}
