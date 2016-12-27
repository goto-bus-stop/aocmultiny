#pragma once
#include <dplobby.h>
#include <string>

using std::string;
using std::wstring;

namespace dplib {

class DPApplication {
private:
  DPAPPLICATIONDESC* dpDesc;

public:
  /**
   * Create a DPApplication.
   */
  DPApplication (GUID guid);

  /**
   * Free up memory.
   */
  ~DPApplication ();

  /**
   * Set the Application name. This should not be localised. Use the
   * description() method to localise the application name.
   */
  DPApplication* appName (string name);
  DPApplication* appName (wstring name);

  /**
   * Set the name of the executable to be launched.
   */
  DPApplication* filename (string name);
  DPApplication* filename (wstring name);

  /**
   * Set the path where the executable is located.
   */
  DPApplication* path (string name);
  DPApplication* path (wstring name);

  /**
   * Set command line parameters to be passed to the executable when launched by
   * a lobby application.
   */
  DPApplication* commandLine (string name);
  DPApplication* commandLine (wstring name);

  /**
   * Set the localized description of the application. This string will be
   * returned by EnumLocalApplications later. If the description is not set, the
   * appName() is used instead.
   */
  DPApplication* description (string name);
  DPApplication* description (wstring name);

  /**
   * Set the working directory to use when launching the application.
   */
  DPApplication* currentDirectory (string name);
  DPApplication* currentDirectory (wstring name);

  /**
   * Set the name of the launcher executable. The launcher is started by the
   * lobby, and it in turn is responsible for launching the application. The
   * launcher must reside in the same directory as the application.
   */
  DPApplication* launcher (string name);
  DPApplication* launcher (wstring name);

  /**
   * Return the internal APPLICATIONDESC structure. If a launcher is set, this
   * will return a DPAPPLICATIONDESC2 structure. If no launcher is set, this
   * will return a DPAPPLICATIONDESC structure.
   */
  DPAPPLICATIONDESC* unwrap ();

  /**
   * Register the application.
   * Shorthand to `DPLobby::get()->registerApplication(this)`.
   */
  HRESULT register_ ();

  /**
   * Construct a DPApplication instance. Provided for fluent construction i.e.
   * DPApplication::create(guid)->appName("Name")->...;
   */
  static DPApplication* create (GUID guid);
};

}
