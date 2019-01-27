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

    void Export(String filename);

    JSONObject ExportComponents();
    JSONObject ExportMaterials();

    inline void SetExportMode(ExportMode mode){ m_exportMode = mode; }
    inline bool InBlacklistMode() { return m_exportMode == BlackList; }
    inline bool InWhiteListMode() { return m_exportMode == WhiteList; }

    void AddCustomUIFile(const String& filename);

private:
    bool CheckSuperTypes(const TypeInfo* type);



    JSONObject fileRoot;
    JSONArray trees;

    ExportMode m_exportMode;
    HashSet<StringHash> m_listOfComponents;
    HashSet<StringHash> m_listOfSuperClasses;
    Vector<String> m_materialFolders;
    Vector<String> m_techniqueFolders;
    Vector<String> m_customUIFilenames;
};
