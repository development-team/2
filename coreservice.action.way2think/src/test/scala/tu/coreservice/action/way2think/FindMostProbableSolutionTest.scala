package tu.coreservice.action.way2think

import org.scalatest.FunSuite
import org.scalatest.Assertions._
import tu.model.knowledge.domain.Concept
import tu.model.knowledge.{Probability, KnowledgeURI, Resource}
import tu.model.knowledge.primitive.KnowledgeString
import tu.model.knowledge.selector.SelectorRequest
import tu.model.knowledge.communication.{Context, ContextHelper}

/**
 * Created by IntelliJ IDEA.
 * User: adel
 * Date: 30.06.12
 * Time: 12:05
 * To change this template use File | Settings | File Templates.
 */

class FindMostProbableSolutionTest extends FunSuite {

  test("One entry should work") {

    org.scalatest.Assertions.expect(1 > 0)(true) //expected, actual
    val w2t0 = new SelectorRequest(KnowledgeURI("test1k"), KnowledgeURI("test1"), new Probability(0.9))

    var context0: Context = ContextHelper(Nil, "test context")
    context0.ClassificationResultsAdd(w2t0)
    val context1 = FindMostProbableSolution(context0)

    org.scalatest.Assertions.expect(w2t0)(context1.LastResult) //expected, actual
    org.scalatest.Assertions.expect(Nil)(context1.ClassificationResults) //expected, actual

    val context2 = FindMostProbableSolution(context1)
    org.scalatest.Assertions.expect(null)(context1.LastResult) //expected, actual
  }


  test("Multi entry should work") {

    org.scalatest.Assertions.expect(1 > 0)(true) //expected, actual
    val w2t0 = new SelectorRequest(KnowledgeURI("test1k"), KnowledgeURI("test1"), new Probability(0.9))
    val w2t1 = new SelectorRequest(KnowledgeURI("test2k"), KnowledgeURI("test2"), new Probability(0.8))

    val context0: Context = ContextHelper(Nil, "test context")

    context0.ClassificationResultsAdd(w2t0)
    context0.ClassificationResultsAdd(w2t1)

    val context1 = FindMostProbableSolution(context0)

    org.scalatest.Assertions.expect(w2t0)(context1.LastResult) //expected, actual
    org.scalatest.Assertions.expect(w2t1)(context1.ClassificationResults.head) //expected, actual

    val context2 = FindMostProbableSolution(context1)

    org.scalatest.Assertions.expect(w2t1)(context2.LastResult) //expected, actual

    context0.ClassificationResultsAdd(w2t1)
    context0.ClassificationResultsAdd(w2t0)

    val context3 = FindMostProbableSolution(context0)

    org.scalatest.Assertions.expect(w2t0)(context3.LastResult) //expected, actual
    org.scalatest.Assertions.expect(w2t1)(context3.ClassificationResults.head) //expected, actual


  }

}