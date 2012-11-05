package tu.model.knowledge.helper

import tu.model.knowledge._
import collection.mutable.ListBuffer

/**
 * 
 * @author: Alexander Toschev
 * Date: 11/3/12
 * Time: 12:15 AM
 * 
 */
object ModelHelper {

  /**
   * check if resource already saved, if yes just create link
   * @param kb  reference to active KB
   * @param parent  parent node
   * @param key
   * @param linkType
   * @param saved
   * @param refObject this node id
   * @param uri uri of this node
   * @return  true if already saved
   */
   def checkIfSaved(kb: KB, parent: KBNodeId, key: String, linkType: String, saved: ListBuffer[String], refObject:KBNodeId,uri:KnowledgeURI): Boolean = {
    val uriC =uri.toString
    if (saved.contains(key)) {
      //only create link
      kb.createLink(parent, refObject, linkType, key)
      return true
    }
    saved.append(key)
    return false
   }
}