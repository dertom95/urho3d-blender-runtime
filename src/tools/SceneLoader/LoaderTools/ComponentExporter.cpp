#include "ComponentExporter.h"

#include <Urho3D/Urho3DAll.h>
#include "base64.h"


Urho3DNodeTreeExporter::Urho3DNodeTreeExporter(Context* context, ExportMode exportMode)
    : Object(context),
      m_exportMode(exportMode)
{
}

void Urho3DNodeTreeExporter::AddComponentHashToFilterList(const StringHash& componentHash)
{
    m_listOfComponents.Insert(componentHash);
}

void Urho3DNodeTreeExporter::AddSuperComponentHashToFilterList(const StringHash& superComponentHash)
{
    m_listOfSuperClasses.Insert(superComponentHash);
}

bool Urho3DNodeTreeExporter::CheckSuperTypes(const TypeInfo* type)
{
    for (auto superType : m_listOfSuperClasses){
        if (!type->IsTypeOf(superType)){
            return false;
        }
    }
    return true;
}

JSONObject Urho3DNodeTreeExporter::ExportMaterials()
{
    const String treeID="urho3dmaterials";

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    FileSystem* fs = GetSubsystem<FileSystem>();

    Vector<String> materialFiles;
    Vector<String> techniqueFiles;

    for (String resDir : cache->GetResourceDirs()){
        Vector<String> dirFiles;
        for (String path : m_materialFolders){
            String dir = resDir+path;
            fs->ScanDir(dirFiles,dir,"*.xml",SCAN_FILES,true);
            for (String foundMaterial : dirFiles){
                auto materialResourceName = path+"/"+foundMaterial;
                Material* material = cache->GetResource<Material>(materialResourceName);
                if (material){
                    materialFiles.Push(materialResourceName);
                }
            }
        }

        // grab techniques from the specified technique folders
        for (String path : m_techniqueFolders){
            String dir = resDir+path;
            fs->ScanDir(dirFiles,dir,"*.xml",SCAN_FILES,true);
            for (String foundTechnique : dirFiles){
                auto techiqueResourceName = path+"/"+foundTechnique;
                Technique* technique = cache->GetResource<Technique>(techiqueResourceName);
                if (technique){
                    techniqueFiles.Push(techiqueResourceName);
                }
            }
        }
    }

    JSONObject tree;

    tree["id"]=treeID;
    tree["name"]="Tree "+treeID;
    tree["icon"]="COLOR_RED";

    JSONArray nodes;

    {
        // create material nodes
        JSONObject materialNode;
        materialNode["id"]=treeID+"__materialNode";
        materialNode["name"]="MaterialNode";
        materialNode["category"]="Material";
        JSONArray props;


        JSONObject materialChooser;
        materialChooser["name"]="Material";
        materialChooser["type"]="enum";
        materialChooser["default"]="0";

        JSONArray enumElems;
        for (String matName : materialFiles){
            JSONObject elem;
            elem["id"]=matName;
            elem["name"]=matName;
            elem["description"]="Material "+matName;
            elem["icon"]="MATERIAL";
            enumElems.Push(elem);
            URHO3D_LOGINFOF("MATERIAL: %s",matName.CString());
        }
        materialChooser["elements"]=enumElems;

        props.Push(materialChooser);
        materialNode["props"]=props;

        nodes.Push(materialNode);
    }
    {
        // create technique nodes
        JSONObject techniqueNode;
        techniqueNode["id"]=treeID+"__techniqueNode";
        techniqueNode["name"]="TechniqueNode";
        techniqueNode["category"]="Material";
        JSONArray props;


        JSONObject techniqueChooser;
        techniqueChooser["name"]="Technique";
        techniqueChooser["type"]="enum";
        techniqueChooser["default"]="0";

        JSONArray enumElems;
        for (String techniqueName : techniqueFiles){
            JSONObject elem;
            elem["id"]=techniqueName;
            elem["name"]=techniqueName;
            elem["description"]="Technique "+techniqueName;
            elem["icon"]="COLOR";
            enumElems.Push(elem);
        }
        techniqueChooser["elements"]=enumElems;

        props.Push(techniqueChooser);
        techniqueNode["props"]=props;

        nodes.Push(techniqueNode);
    }
    // TODO: create a nodes-builder or similars!?

    tree["nodes"]=nodes;

    return tree;
}

void Urho3DNodeTreeExporter::AddMaterialFolder(const String &folder)
{
    m_materialFolders.Push(folder);
}

void Urho3DNodeTreeExporter::AddTechniqueFolder(const String& folder)
{
    m_techniqueFolders.Push(folder);
}

JSONObject Urho3DNodeTreeExporter::ExportComponents()
{
    const HashMap<StringHash, SharedPtr<ObjectFactory> >& objFactories = context_->GetObjectFactories();

    auto values = objFactories.Values();

    JSONObject tree;

    String treeID = "urho3dcomponents";

    tree["id"]=treeID;
    tree["name"]="Tree "+treeID;
    tree["icon"]="OUTLINER_OB_GROUP_INSTANCE";

    JSONArray nodes;

    Vector<Pair<String, unsigned> > sortedTypes;
    for (unsigned i = 0; i < values.Size(); ++i)
    {
        SharedPtr<ObjectFactory> val = values.At(i);

        // apply black- /whitelist-Filter
        if (    (InBlacklistMode() && (m_listOfComponents.Contains(val->GetType()) || CheckSuperTypes(val->GetTypeInfo())) )
             || (InWhiteListMode() && (!m_listOfComponents.Contains(val->GetType()) && !CheckSuperTypes(val->GetTypeInfo()) )) )
            continue;

        if (val.NotNull())
        {
            sortedTypes.Push(MakePair(val->GetTypeName(), i));
        }
    }
    Sort(sortedTypes.Begin(), sortedTypes.End());

    const HashMap<StringHash, SharedPtr<ObjectFactory> >& all = context_->GetObjectFactories();

    for (int i=0;i < sortedTypes.Size(); i++){

        auto objectFactoryName = sortedTypes[i].first_;
        JSONObject node;

        SharedPtr<ObjectFactory> val = *all[StringHash(objectFactoryName)];

        if (val->GetTypeInfo()->IsTypeOf(Component::GetTypeInfoStatic())){
       //     URHO3D_LOGINFOF("TYPE:%s\n",val->GetTypeName().CString());
            node["category"]="Component";
        } else {
       //     URHO3D_LOGINFOF("NO COMPONENT: TYPE:%s\n",val->GetTypeName().CString());
        }

        node["id"]=treeID+"__"+val->GetTypeName().Replaced(" ","_");
        node["name"]=val->GetTypeName();

        JSONArray props;

        auto attrs = context_->GetAttributes(val->GetTypeInfo()->GetType());
        if (attrs){
            for (int j = 0;j<attrs->Size();j++){
                auto attr = attrs->At(j);
              //  URHO3D_LOGINFOF("\tattr:%s\n", attr.name_.CString());


                JSONObject prop;
                prop["name"] = attr.name_;
                switch (attr.type_){
                    case VAR_BOOL : prop["type"]="bool"; prop["default"]=attr.defaultValue_.ToString(); break;
                    case VAR_INT : {
                        if (!attr.enumNames_) {
                            prop["type"]="int"; prop["default"]=attr.defaultValue_.ToString();
                        } else {
                            prop["type"]="enum"; prop["default"]=attr.defaultValue_.ToString();

                            JSONArray elements;
                            for (int idx = 0; attr.enumNames_[idx] != NULL; idx++)
                            {
                                JSONObject elem;
                                elem["id"]=attr.enumNames_[idx];
                                elem["name"]=attr.enumNames_[idx];
                                elements.Push(elem);
                            }

                            prop["elements"]=elements;
                        }
                        break;
                    }
                    case VAR_FLOAT : prop["type"]="float"; prop["default"]=attr.defaultValue_.ToString();break;
                    case VAR_STRING : prop["type"]="string"; prop["default"]=attr.defaultValue_.ToString();break;
                    case VAR_COLOR : prop["type"]="color"; prop["default"]="("+attr.defaultValue_.ToString().Replaced(" ",",")+")";break;
                    case VAR_VECTOR2 : prop["type"]="vector2";prop["default"]="("+attr.defaultValue_.ToString().Replaced(" ",",")+")"; break;
                    case VAR_VECTOR3 : prop["type"]="vector3";prop["default"]="("+attr.defaultValue_.ToString().Replaced(" ",",")+")"; break;
                    case VAR_VECTOR4 : prop["type"]="vector4";prop["default"]="("+attr.defaultValue_.ToString().Replaced(" ",",")+")"; break;
                    default:
                        URHO3D_LOGINFOF("[%s] Skipping attribute:%s",val->GetTypeName().CString(),attr.name_.CString());
                        continue;

                }

                props.Push(prop);
            }
        }
        node["props"]=props;
        nodes.Push(node);

    }
    tree["nodes"]=nodes;
    return tree;
}

void Urho3DNodeTreeExporter::Export(String filename)
{
    auto componentTree = ExportComponents();
    auto materialTree = ExportMaterials();

    trees.Push(componentTree);
    trees.Push(materialTree);

    if (!m_customUIFilenames.Empty()){
        FileSystem* fs = GetSubsystem<FileSystem>();
        JSONArray jsonCustomUIs;
        for (auto m_customUIFilename : m_customUIFilenames)
        {
            if (!fs->FileExists(m_customUIFilename)){
                URHO3D_LOGERRORF("File %s not found",m_customUIFilename.CString());
            } else {
                File file(context_);
                file.Open(m_customUIFilename);
                String allText="";
                while (!file.IsEof()){
                    String line = file.ReadLine()+"\n";
                    allText += line;
                }
                URHO3D_LOGINFO("READ FILE:");
                URHO3D_LOGINFO(allText.CString());

                auto e = base64_encode(reinterpret_cast<const unsigned char*>(allText.CString()),allText.Length());
                String enc(e.c_str());
                String dec(base64_decode(enc.CString()).c_str());


                jsonCustomUIs.Push(enc);
            }
        }
        fileRoot["customUI"] = jsonCustomUIs;
    }

    fileRoot["trees"]=trees;

    JSONFile file(context_);
    file.GetRoot() = fileRoot;
    file.SaveFile(filename);

}

void Urho3DNodeTreeExporter::AddCustomUIFile(const String &filename)
{
     m_customUIFilenames.Push(filename);
}
