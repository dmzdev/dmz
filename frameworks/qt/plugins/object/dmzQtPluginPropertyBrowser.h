#ifndef DMZ_QT_PLUGIN_PROPERTY_BROWSER_DOT_H
#define DMZ_QT_PLUGIN_PROPERTY_BROWSER_DOT_H

#include <dmzQtWidget.h>
#include "dmzQtPropertyManager.h"
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <QtCore/QMap>
#include <QtGui/QFrame>
#include "ui_PropertyBrowser.h"


class QtProperty;
class QtVariantPropertyManager;
class QtEnumPropertyManager;
class QtGroupPropertyManager;


namespace dmz {

   class QtPluginPropertyBrowser :
         public QFrame,
         public QtWidget,
         public Plugin,
         public MessageObserver,
         public ObjectObserverUtil {

      Q_OBJECT
   
      public:
         QtPluginPropertyBrowser (const PluginInfo &Info, Config &local);
         ~QtPluginPropertyBrowser ();

         // QtWidget Interface
         virtual QWidget *get_qt_widget ();
         
         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Message Observer Interface
         virtual void receive_message (
            const Message &Type,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_uuid (
            const Handle ObjectHandle,
            const UUID &Identity,
            const UUID &PrevIdentity);

         virtual void remove_object_attribute (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &AttrMask);

         virtual void update_object_locality (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectLocalityEnum Locality,
            const ObjectLocalityEnum PrevLocality);

         virtual void link_objects (
            const Handle LinkHandle,
            const Handle AttributeHandle,
            const UUID &SuperIdentity,
            const Handle SuperHandle,
            const UUID &SubIdentity,
            const Handle SubHandle);

         virtual void unlink_objects (
            const Handle LinkHandle,
            const Handle AttributeHandle,
            const UUID &SuperIdentity,
            const Handle SuperHandle,
            const UUID &SubIdentity,
            const Handle SubHandle);

         virtual void update_link_attribute_object (
            const Handle LinkHandle,
            const Handle AttributeHandle,
            const UUID &SuperIdentity,
            const Handle SuperHandle,
            const UUID &SubIdentity,
            const Handle SubHandle,
            const UUID &AttributeIdentity,
            const Handle AttributeObjectHandle,
            const UUID &PrevAttributeIdentity,
            const Handle PrevAttributeObjectHandle);

         virtual void update_object_counter (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Int64 Value,
            const Int64 *PreviousValue);

         virtual void update_object_counter_minimum (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Int64 Value,
            const Int64 *PreviousValue);

         virtual void update_object_counter_maximum (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Int64 Value,
            const Int64 *PreviousValue);

         virtual void update_object_alternate_type (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const ObjectType &Value,
            const ObjectType *PreviousValue);

         virtual void update_object_state (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &Value,
            const Mask *PreviousValue);

         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

         virtual void update_object_time_stamp (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 &Value,
            const Float64 *PreviousValue);

         virtual void update_object_position (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_orientation (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Matrix &Value,
            const Matrix *PreviousValue);

         virtual void update_object_velocity (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_acceleration (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_scale (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_vector (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_scalar (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue);

         virtual void update_object_text (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const String &Value,
            const String *PreviousValue);

         virtual void update_object_data (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Data &Value,
            const Data *PreviousValue);

      protected slots:
         void _value_changed (QtProperty *property, const QVariant &value);
         
         void on_objectTreeWidget_currentItemChanged (
            QTreeWidgetItem *current, QTreeWidgetItem *previous);
      
      protected:
         QtProperty *_lookup_group_property (const QString &Name);

         void _add_int64_property (
               const QString &GroupName,
               const Handle AttributeHandle,
               const Int64 Value);
         
         void _add_float64_property (
               const QString &GroupName,
               const Handle AttributeHandle,
               const Float64 Value);

         void _add_string_property (
               const QString &GroupName,
               const Handle AttributeHandle,
               const QString &Value);

         void _add_vector_property (
               const QString &GroupName,
               const Handle AttributeHandle,
               const Vector &Value);

         void _add_property (QtProperty *property, const QString &id);
         void _update_expand_state ();
         QString _uuid_to_string (const UUID &Identity);
         QString _type_to_string (const ObjectType &Type);
         QString _handle_to_name (const Handle Object);
         QString _handle_to_string (const Handle Object);
         Handle _item_to_handle (QTreeWidgetItem *item);
         void _init (Config &local);

         Log _log;
         Definitions _defs;
         Ui::PropertyBrowserForm _ui;
         Handle _defaultAttrHandle;
         QtGroupPropertyManager *_groupManager;
         QtEnumPropertyManager *_enumManager;
         QtEnumPropertyManager *_enumManagerRO;
         QtVariantPropertyManager *_variantManager;
         QtVariantPropertyManager *_variantManagerRO;
         VectorPropertyManager *_vectorManager;
         VectorPropertyManager *_vectorManagerRO;
         
         QMap<QtProperty *, QString> _propertyToId;
         QMap<QString, QtProperty *> _idToProperty;
         QMap<QString, bool> _idToExpanded;
         QTreeWidgetItem *_currentItem;
         Handle _currentObject;

      private:
         QtPluginPropertyBrowser ();
         QtPluginPropertyBrowser (const QtPluginPropertyBrowser &);
         QtPluginPropertyBrowser &operator= (const QtPluginPropertyBrowser &);

   };
};

#endif // DMZ_QT_PLUGIN_PROPERTY_BROWSER_DOT_H