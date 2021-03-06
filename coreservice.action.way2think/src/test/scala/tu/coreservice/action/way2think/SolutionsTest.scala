package tu.coreservice.action.way2think

import org.junit.runner.RunWith
import org.scalatest.junit.JUnitRunner
import org.scalatest.FunSuite
import tu.model.knowledge.domain.{ConceptLink, Concept, ConceptNetwork}
import tu.model.knowledge.semanticnetwork.{SemanticNetwork, SemanticNetworkLink, SemanticNetworkNode}
import tu.coreservice.utilities.TestDataGenerator
import tu.model.knowledge.howto.{HowTo, Solution}
import tu.model.knowledge.primitive.{KnowledgeBoolean, KnowledgeString}
import tu.model.knowledge._
import narrative.Rule

/**
 * Created by IntelliJ IDEA.
 * User: alex toschev
 * Date: 11.07.12
 * Time: 2:30
 * To change this template use File | Settings | File Templates.
 */

@RunWith(classOf[JUnitRunner])
class SolutionsTest extends FunSuite{

  val uri = new KnowledgeURI("namespace", "name", "revision")
  val probability = new Probability

  val si1 = getTestSolvedIssue1
  val si2 = getTestSolvedIssue2
  val si3 = getTestSolvedIssue3

  val searcher = new Solutions

  searcher.solutions = Nil

  searcher.add(si1)
  searcher.add(si2)
  searcher.add(si3)

  test("Solutions can search one-node network") {

    //may be it is firefox
    val net1 = new ConceptNetwork(List[Concept](getTestSolvedIssue1.issue.rootNodes(1)), Nil, KnowledgeURI("pleaseInstallFFTest1"))
    //may be it is IE8
    val net2 = new ConceptNetwork(List[Concept](getTestSolvedIssue2.issue.rootNodes(1)), Nil, KnowledgeURI("pleaseInstallFFTest1"))

    val ssi1 = searcher.search ( net1, Nil)
    val ssi2 = searcher.search ( net2, Nil)

    ssi1 match {
      case Some(si: SolvedIssue) => {
        assertResult(si.issue.uri)(si1.issue.uri)
      }
      case _ => {
        //Unexpected type or None instead Some
        assertResult(0)(1)
      }
    }

    ssi2 match {
      case Some(si: SolvedIssue) => {
        assertResult(si.issue.uri)(si2.issue.uri)
      }
      case _ => {
        //Unexpected type or None instead Some
        assertResult(0)(1)
      }
    }
  }

  test("Solutions can search exact network") {

    val net1 = getTestSolvedIssue1.issue
    val net3 = getTestSolvedIssue3.issue

    val ssi1 = searcher.search ( net1, Nil)
    val ssi3 = searcher.search ( net3, Nil)

    ssi1 match {
      case Some(si: SolvedIssue) => {
        assertResult(si.issue.uri)(si1.issue.uri)
      }
      case _ => {
        //Unexpected type or None instead Some
        assertResult(0)(1)
      }
    }

    ssi3 match {
      case Some(si: SolvedIssue) => {
        assertResult(si.issue.uri)(si3.issue.uri)
      }
      case _ => {
        //Unexpected type or None instead Some
        assertResult(0)(1)
      }
    }
  }



  def getTestSolvedIssue1(): SolvedIssue = {
    val s = new Solution(List(TestDataGenerator.generateInstallFirefoxHowTo), uri)
    new SolvedIssue(TestDataGenerator.pleaseInstallFFSimulation, s, uri, probability)
  }

  def getTestSolvedIssue2(): SolvedIssue = {
    val s = new Solution(List(TestDataGenerator.generateReinstallIE8HowTo), uri)
    new SolvedIssue(TestDataGenerator.iHaveProblemWithIE8Simulation, s, uri, probability)
  }

  def getTestSolvedIssue3(): SolvedIssue = {
    val s = new Solution(List(TestDataGenerator.generateReinstallIE8HowTo), uri)
    new SolvedIssue(TestDataGenerator.iHaveProblemWithIE8Reformulation, s, uri, probability)
  }

}