--------------------------------------------------------------------------------

 MantraForKatana
 Copyright (c) 2014-2019, Davide Selmo
 See LICENSE.txt for the license text

--------------------------------------------------------------------------------

 This software is provided "as is", and is entirely unconnected to any
 development work done by The Foundry or Side Effects.

 Please don't use the usual The Foundry or Side Effects support channels
 for any questions or issues relating to this software.
 Email ds_gfx@zoho.com instead.

 All trademarks are the properties of their respective holders.

--------------------------------------------------------------------------------


MantraForKatana is a renderer plug-in for The Foundry's Katana that allows
users to render using Side Effects' Mantra renderer.


Install and usage instructions
------------------------------

 - Set up the build environment:

   - Set the KATANA_HOME environment variable
   - Initialize the Houdini environment

 - Build and install the plug-ins:

   - Change directory to src/ and run
     make && make install

   - Add the content of the VRAYprocedural file in src/Procedural to
     the content of the VRAYprocedural file in your ~/houdinix.x folder

 - Set up the application environment

   - Set the KATANA_RESOURCES environment variable to include the
     Resources sub-folder

   - Set the LD_LIBRARY_PATH to include Houdini libraries search path.
     If the Houdini environment is correctly set, the $HDSO variable can
     be used

 Tested with Katana 1.5v1,1.5v2 and Houdini 13.0.198.21


Current limitations
-------------------

 - No support for display drivers yet, all renders will be targeted to MPlay

 - Basic for Mantra shaders.
   The RendererInfo plug-in is very basic and it only implements methods
   needed to advertise the renderer plug-in.

 - Support for polymesh and subdmesh locations only

 - Basic support for GenericAssign global settings

 - No support for GenericAssign object settings


Know issues
-----------

 - HDF5 conflicts
   Katana and Mantra use different versions of Alembic and HFD5, this could
   make your renders fail. If that's the case, disable the HDF5 version
   check using:

   export HDF5_DISABLE_VERSION_CHECK=1

   Alternatively the Katana Alembic_In plug-in can be re-built linking
   against Alembic libraries provided with Houdini

 - ZLib conflicts
   Katana and Mantra use different versions of ZLib. If you experience crashes
   when saving your scene try to force Katana's ZLib to be loaded first by
   setting the LD_PRELOAD environment variable when starting Katana:

   LD_PRELOAD=$KATANA_HOME/bin/ZLib/libz.so $KATANA_HOME/katana
