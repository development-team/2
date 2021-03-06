package tu.model.knowledge.annotator

import tu.model.knowledge.domain.Concept
import tu.model.knowledge.{Resource, Probability, KnowledgeURI}
import tu.model.knowledge.primitive.KnowledgeString

/**
 * @author alex
 *         Time stamp: 6/29/12 11:28 AM
 */

case class Phrase(  val _value: String,  val _uri: KnowledgeURI, val _probability: Probability)
  extends KnowledgeString(_value, _uri, _probability) {

  def this( _value: String, _uri: KnowledgeURI) {
    this( _value, _uri, new Probability)
  }



}

object Phrase {

  def apply(name: String):Word = {
    val uri: KnowledgeURI = KnowledgeURI(name)
    new Word( name, uri)
  }

}