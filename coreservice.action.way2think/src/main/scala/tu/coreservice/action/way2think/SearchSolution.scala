package tu.coreservice.action.way2think

import tu.model.knowledge.communication.{ContextHelper, Context}
import tu.model.knowledge.domain.ConceptNetwork
import tu.model.knowledge.howto.Solution
import tu.model.knowledge.{Resource, SolvedIssue, Solutions}

/**
 * @author adel
 * Date: 10.07.12
 * Time: 7:00
 */

object SearchSolution {

  def apply(inputContext: Context): Context = {

    // TODO fix this !
    // val res = search

    // val outputContext = ContextHelper(res match{ case Some(x) => List[Resource](x) case None => List[Resource]() }, "OutputContex")
    // outputContext.lastResult = res
    // outputContext
    null
  }

  def search(target: ConceptNetwork, solutions:Solutions):SolvedIssue = {
    solutions.search(target, Nil)
    null
  }
}