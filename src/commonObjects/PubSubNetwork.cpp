//
// Copyright (c) 2008-2019 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "PubSubNetwork.h"

#include <Urho3D/Urho3DAll.h>
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <CommonEvents.h>


PubSubMessage::PubSubMessage(Context* ctx, const zmq::multipart_t& msg)
    : ctx_(ctx)
{
    msg_ = msg.clone();
    topic_ = msg_.peekstr(0).c_str();
}


VectorBuffer PubSubMessage::PopData()
{
    zmq::message_t zmq_msg = msg_.pop();
    VectorBuffer vbuf(zmq_msg.data<char>(),zmq_msg.size());
    return vbuf;
}

String PubSubMessage::PopString()
{
    String incomingString(msg_.popstr().c_str());
    return incomingString;
}

JSONObject PubSubMessage::PopJson()
{
    String popStr = PopString();
    JSONFile jsonFile(ctx_);
    jsonFile.FromString(popStr);
    return jsonFile.GetRoot().GetObject();
}


PubSubNetwork::PubSubNetwork(Context* context) :
    Object(context)
    ,running_(false)
    ,initialized_(false)
{
    SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(PubSubNetwork, HandleBeginFrame));
}

void PubSubNetwork::RegisterObject(Context* context)
{
    //context->RegisterFactory<Network>("Sample Component");

//    // These macros register the class attributes to the Context for automatic load / save handling.
//    // We specify the Default attribute mode which means it will be used both for saving into file, and network replication
//    URHO3D_ATTRIBUTE("BOOL VALUE", bool, boolValue_, DEFAULT_BOOL_VALUE, AM_FILE);
//    URHO3D_ATTRIBUTE("INT VALUE", int, intValue_, DEFAULT_INT_VALUE, AM_FILE);
//    URHO3D_ATTRIBUTE("FLOAT VALUE", float, floatValue_, DEFAULT_FLOAT_VALUE, AM_FILE);
//    URHO3D_ATTRIBUTE("STRING VALUE", String, stringValue_, DEFAULT_STRING_VALUE, AM_FILE);
//    //URHO3D_ATTRIBUTE("Anim Run", String, animRun_, "", AM_DEFAULT);
//    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Animation", GetAnimation, SetAnimation, ResourceRef, ResourceRef(Animation::GetTypeStatic()), AM_DEFAULT);
//    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Texture", GetTexture, SetTexture, ResourceRef, ResourceRef(Texture2D::GetTypeStatic()), AM_DEFAULT);
}

void PubSubNetwork::InitNetwork(const String& host, const String& initialFilter,int portIn, int portOut)
{
    inSocket_ = zmq::socket_t(ctx, zmq::socket_type::sub);
    inSocket_.connect(("tcp://"+host+":"+String(portIn)).CString());

    inSocket_.setsockopt(ZMQ_SUBSCRIBE, initialFilter.CString(),initialFilter.Length());
    //inSocket_.setsockopt(ZMQ_RCVTIMEO, 0);

    outSocket_ = zmq::socket_t(ctx, zmq::socket_type::pub);
    outSocket_.connect(("tcp://"+host+":"+String(portOut)).CString());

}

void PubSubNetwork::CheckNetwork()
{
    zmq::multipart_t multipart;

    auto ok = multipart.recv(inSocket_, ZMQ_DONTWAIT);
    if (ok){
        PubSubMessage msg(context_,multipart);

        using namespace NSPubSubMessage;
        VariantMap map;
        map[P_TOPIC]=msg.GetTopic();
        map[P_MSG]=MakeCustomValue(&msg);
        SendEvent(E_PUBSUB_MSG,map);
    }

}

void PubSubNetwork::HandleBeginFrame(StringHash eventType, VariantMap &eventData)
{
    CheckNetwork();
}

void PubSubNetwork::Close()
{
    inSocket_.close();
    outSocket_.close();
    ctx.close();
}

void PubSubNetwork::Send(const String& topic,const String& txtData,void* buffer,int length)
{
    zmq::multipart_t multipart;
    multipart.addstr(topic.CString());
    multipart.addstr(txtData.CString());
    if (buffer && length){
        multipart.add(zmq::message_t(buffer,length));
    }
    multipart.send(outSocket_);
}
void PubSubNetwork::Send(const String& topic,const StringVector& txtData,void* buffer,int length)
{
    zmq::multipart_t multipart;

    multipart.addstr(topic.CString());
    for (const String  txt : txtData){
        multipart.addstr(txt.CString());
    }
    if (buffer && length){
        multipart.add(zmq::message_t(buffer,length));
    }
    multipart.send(outSocket_);

}

void PubSubNetwork::Send(const String& topic,const String& subtype, void *buffer,int length, const String& meta)
{
    zmq::multipart_t multipart;
    multipart.addstr((topic+" "+subtype+" bin").CString());
    multipart.addstr(meta.CString());
    multipart.add(zmq::message_t(buffer,length));
    multipart.send(outSocket_);
}

void PubSubNetwork::Send(const String& topic,const String& subtype, const String& txtData, const String& meta)
{
    zmq::multipart_t multipart;
//    multipart.push(zmq::message_t(topic.CString(),topic.Length()));
//    multipart.push(zmq::message_t(txtData.CString(),topic.Length()));
    multipart.addstr((topic+" "+subtype+" text").CString());
    multipart.addstr(meta.CString());
    multipart.addstr(txtData.CString());
    multipart.send(outSocket_);
}

//void PubSubNetwork::CreateScreenshot()
//{
//    if (additionalResourcePath=="") return;

//    Graphics* graphics = GetSubsystem<Graphics>();
//    Image screenshot(context_);
//    graphics->TakeScreenShot(screenshot);
//    // Here we save in the Data folder with date and time appended
////    screenshot.SavePNG(GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Screenshot_" +
////        Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_') + ".png");
//    String screenshotPath = additionalResourcePath+"/Screenshot.png";
//    screenshot.SavePNG(screenshotPath);
//   // URHO3D_LOGINFOF("Save screenshot to %s",screenshotPath.CString());
//}







