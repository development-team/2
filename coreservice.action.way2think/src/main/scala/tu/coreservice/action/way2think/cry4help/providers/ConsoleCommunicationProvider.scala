package tu.coreservice.action.way2think.cry4help.providers

import tu.coreservice.action.way2think.cry4help.Cry4HelpCommunicationProvider
import com.typesafe.config.ConfigFactory
import org.slf4j.LoggerFactory


/**
 *
 * @author Alexander Toschev
 *         Date: 9/9/12
 *         Time: 7:06 PM
 *
 */

class ConsoleCommunicationProvider extends Cry4HelpCommunicationProvider {

  val log = LoggerFactory.getLogger(this.getClass)

  /**
   *
   * @param quest quest to user
   * @return response from user
   */
  def askQuestion(quest: String): String = {
    showInfo(quest)
    //try to identify if console is availible


    var consoleAvailable = "false"
    try {
      val conf = ConfigFactory.load("application.properties")
      consoleAvailable = conf.getString("coreservice.action.way2think.cry4help.console")
    }
    catch {
      case ex: Exception => log.error("unable load config")
    }
    if (consoleAvailable == "true") {
      log.debug("Console is availible. Read from tu.coreservice.action.way2think.cry4help.console. Using input")
      Console.println("You :>")
      Console.readLine()
    }
    else {
      log.debug("Console is not availible. Returning empty string")
      ""
    }

  }

  private def showInfo(in: String) {
    Console.println(in)
  }

  /**
   * just print information without any output
   * @param inf to be indicated in Console
   */
  def showInformation(inf: String) {
    showInfo(inf)
  }
}
