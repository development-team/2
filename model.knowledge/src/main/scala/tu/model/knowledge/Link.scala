package tu.model.knowledge


/**
 * @author adel
 *         date 2012-05-02
 *         time: 10:41 PM
 */

abstract class Link[T <: Resource](_source: T, _destination: T, _uri: KnowledgeURI, _probability: Probability = new Probability())
  extends Resource(_uri, _probability) {

  def source: T = _source

  def destination: T = _destination

}
