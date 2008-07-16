#include <dmzObjectAttributeMasks.h>
#include <dmzRenderModuleCoreOSG.h>
#include "dmzRenderPluginObjectOSG.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeObjectType.h>
#include <osgDB/ReadFile>

dmz::RenderPluginObjectOSG::RenderPluginObjectOSG (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defs (Info, &_log),
      _core (0) {

   _init (local);
}


dmz::RenderPluginObjectOSG::~RenderPluginObjectOSG () {

   _modelTable.empty ();
   _typeTable.clear ();
   _defTable.empty ();
   _objectTable.empty ();
}


// Plugin Interface
void
dmz::RenderPluginObjectOSG::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::RenderPluginObjectOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) { _core = RenderModuleCoreOSG::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOSG::cast (PluginPtr))) { _core = 0; }
   }
}


// Object Observer Interface
void
dmz::RenderPluginObjectOSG::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (_core) {

      DefStruct *ds (_lookup_def_struct (Type));

      if (ds) {

         ObjectStruct *os (new ObjectStruct (*ds));

         if (os && os->model.valid ()) {

            os->model->setSingleChildOn (0);

            osg::Group *g (_core->create_dynamic_object (ObjectHandle));

            if (g) { g->addChild (os->model.get ()); }
         }
      }
   }
}


void
dmz::RenderPluginObjectOSG::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   ObjectStruct *os (_objectTable.remove (ObjectHandle));

   if (os) { delete os; os = 0; }
}


void
dmz::RenderPluginObjectOSG::update_object_type (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const ObjectType &Value,
      const ObjectType *PreviousValue) {

   ObjectStruct *os (_objectTable.lookup (ObjectHandle));

   if (os && _core) {

      DefStruct *ds (_lookup_def_struct (Value));

      if (ds != &(os->Def)) {

         destroy_object (Identity, ObjectHandle);
         create_object (Identity, ObjectHandle, Value, ObjectLocalityUnknown);
      }
   }
}


void
dmz::RenderPluginObjectOSG::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   ObjectStruct *os (_objectTable.lookup (ObjectHandle));

   if (os && os->model.valid ()) {

      unsigned int place (0);

      StateStruct *ss (os->Def.stateMap);

      while (ss) {

         if (Value.contains (ss->State)) { place = ss->Place; ss = 0; }
         else { ss = ss->next; }
      }

      os->model->setSingleChildOn (place);
   }
}


void
dmz::RenderPluginObjectOSG::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

}


void
dmz::RenderPluginObjectOSG::update_object_scale (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

}


dmz::RenderPluginObjectOSG::DefStruct *
dmz::RenderPluginObjectOSG::_lookup_def_struct (const ObjectType &Type) {

   DefStruct *result (_typeTable.lookup (Type.get_handle ()));

   if (!result) {

      result = _create_def_struct (Type);

      if (result) { _typeTable.store (Type.get_handle (), result); }
   }

   return result;
}


dmz::RenderPluginObjectOSG::DefStruct *
dmz::RenderPluginObjectOSG::_create_def_struct (const ObjectType &Type) {

   DefStruct *result (0);

   ObjectType currentType (Type);

   while (!result && currentType) {

      Config modelList;

      if (currentType.get_config ().lookup_all_config ("render.model", modelList)) {

         result = new DefStruct;

         if (_defTable.store (currentType.get_handle (), result)) {

            ConfigIterator it;
            Config model;

            unsigned int place (1);

            StateStruct *currentState (0);

            while (modelList.get_next_config (it, model)) {

               const String FileName (config_to_string ("file", model));
               Mask state;
               String stateName;
               const Boolean StateNameFound (model.lookup_attribute ("state", stateName));

               if (StateNameFound) { _defs.lookup_state (stateName, state); }

               if (!StateNameFound || state) {

                  ModelStruct *ms = _load_model (FileName);

                  if (ms) {

                     unsigned int switchPlace (StateNameFound ? place : 0);
                     if (StateNameFound) { place++; }

                     if (((switchPlace + 1) > result->model->getNumChildren ()) ||
                           !result->model->getChild (switchPlace)) {

                        result->model->insertChild (switchPlace, ms->model.get ());

                        if (switchPlace) {

                           StateStruct *ss (new StateStruct (switchPlace, state));

                           if (currentState) {

                              currentState->next = ss;
                              currentState = ss;
                           }
                           else { result->stateMap = currentState = ss; }
                        }
                     }
                  }
               }
            }
         }
         else { delete result; result = 0; }
      }

      currentType.become_parent ();

      if (!result) { result = _defTable.lookup (currentType.get_handle ()); }
   }

   return result;
}


dmz::RenderPluginObjectOSG::ModelStruct *
dmz::RenderPluginObjectOSG::_load_model (const String &FileName) {

   ModelStruct *result (_modelTable.lookup (FileName));

   if (!result) {

      result = new ModelStruct;
      result->model = osgDB::readNodeFile (FileName.get_buffer ());

      if (!result->model.valid ()) {

         delete result; result = 0;
         _log.error << "Failed loading file: " << FileName << endl;
      }
   }

   return result;
}


void
dmz::RenderPluginObjectOSG::_init (Config &local) {

   activate_default_object_attribute (
      ObjectCreateMask |
      ObjectDestroyMask |
      ObjectTypeMask |
      ObjectStateMask);

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginObjectOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginObjectOSG (Info, local);
}

};