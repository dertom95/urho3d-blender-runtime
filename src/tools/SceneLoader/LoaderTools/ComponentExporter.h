#pragma once

#include <Urho3D/Core/Object.h>
#include <Urho3D/Math/StringHash.h>
#include <Urho3D/Container/Str.h>
#include <Urho3D/Container/HashSet.h>
#include <Urho3D/Resource/JSONValue.h>

//#define BTH_DEBUG

namespace Urho3D {
    class Context;

}

using namespace Urho3D;


class Urho3DNodeTreeExporter : public Object {
    URHO3D_OBJECT(Urho3DNodeTreeExporter, Object);
public:
    enum ExportMode { WhiteList, BlackList };

    Urho3DNodeTreeExporter(Context* context,ExportMode mode=BlackList);


    void AddComponentHashToFilterList(const StringHash& componentHash);
    // add/discard components that have this component as superclass
    void AddSuperComponentHashToFilterList(const StringHash& componentHash);

    void AddMaterialFolder(const String& folder);
    void AddTechniqueFolder(const String& folder);
    void AddTextureFolder(const String& folder);

    void Export(String filename);

    JSONObject ExportComponents();
    JSONObject ExportMaterials();

    inline void SetExportMode(ExportMode mode){ m_exportMode = mode; }
    inline bool InBlacklistMode() { return m_exportMode == BlackList; }
    inline bool InWhiteListMode() { return m_exportMode == WhiteList; }

    void AddCustomUIFile(const String& filename);


    enum NodeType {NT_BOOL,NT_FLOAT,NT_INT,NT_STRING,NT_VECTOR2,NT_VECTOR3,NT_VECTOR4,NT_COLOR};
    enum NodeSubType {ST_NONE,ST_PIXEL,ST_UNSIGNED,ST_FACTOR,ST_ANGLE,ST_TIME,ST_DISTANCE};
    enum NodeSocketType {SOCK_FLOAT,SOCK_BOOL,SOCK_STRING,SOCK_VECTOR};

    void NodeSetData(JSONObject& node,const String& id,const String& name,const String category="misc");
    void NodeAddProp(JSONObject& node, const String& name, NodeType type, const String& defaultValue, NodeSubType subType=ST_NONE, int precission=3, float min=0, float max=1.0f);
    void NodeAddPropEnum(JSONObject& node,const String& name,JSONArray& elements,const String& defaultValue="0");
    void NodeAddEnumElement(JSONArray& elementsArray, const String& id,const String& name="",const String& descr="",const String& icon="COLOR",const String& number="0");
    void NodeAddInputSocket(JSONObject& node,const String& name, NodeSocketType type);
    void NodeAddOutputSocket(JSONObject& node,const String& name, NodeSocketType type);
private:
    void NodeAddSocket(JSONObject& node,const String& name, NodeSocketType type, bool isInputSocket);

    bool CheckSuperTypes(const TypeInfo* type);
    String GetTypeCategory(const StringHash& hash,const String& defaultValue);


    JSONObject fileRoot;
    JSONArray trees;

    ExportMode m_exportMode;
    HashSet<StringHash> m_listOfComponents;
    HashSet<StringHash> m_listOfSuperClasses;
    Vector<String> m_materialFolders;
    Vector<String> m_techniqueFolders;
    Vector<String> m_textureFolders;
    Vector<String> m_customUIFilenames;
};
