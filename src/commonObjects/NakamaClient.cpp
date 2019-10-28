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

#include "NakamaClient.h"

#include <Urho3D/Urho3DAll.h>

#include <nakama-cpp/Nakama.h>
#include <commonObjects/NakamaClientEvents.h>

using namespace Nakama;

NakamaClient::NakamaClient(Context* context) :
    Object(context),
    authenticated_(false)

{
    SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(NakamaClient, HandleBeginFrame));
}


//void PubSubNetwork::RegisterObject(Context* context)
//{
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
//}



void NakamaClient::Connect(const String &host, int port, const String &serverKey)
{
    NClientParameters parameters;
    parameters.serverKey = serverKey.CString();
    parameters.host = host.CString();
    parameters.port = port;
    client_ = createDefaultClient(parameters);
}

void NakamaClient::Authenticate(const String &email, const String &password, bool createIfNotExisting)
{

    auto successCallback = [this](NSessionPtr session)
    {
        URHO3D_LOGINFOF("nakama: session token %s",session->getAuthToken().c_str());
        this->session_ = session;
        this->authenticated_=true;
        //std::cout << "session token: " << session->getAuthToken() << std::endl;

        bool createStatus = true; // if the server should show the user as online to others.
        // define realtime client in your class as NRtClientPtr rtClient;

        rtClient_ = client_->createRtClient(DEFAULT_PORT);
        // define listener in your class as NRtDefaultClientListener listener;
        listener_.setConnectCallback([]()
        {
            URHO3D_LOGINFO("SOCKET CONNECTED");
        });

        listener_.setMatchmakerMatchedCallback([](NMatchmakerMatchedPtr matched)
        {
            auto token = matched->token;
            auto users = matched->users;
            auto matchId = matched->matchId;
            int a=0;
            URHO3D_LOGINFOF("token:%s useramount:%d matchid:%s",token.c_str(),users.size(),matchId.c_str());
        });

        rtClient_->setListener(&listener_);
        rtClient_->connect(session_, createStatus);
    };

    auto errorCallback = [](const NError& error)
    {
        URHO3D_LOGINFOF("nakama: error %s",error.message.c_str());
    };

    authenticated_=false;
    //client_->authenticateDevice("123456","tomtomtom",true,successCallback,errorCallback);
    const NStringMap& vars={};
    client_->authenticateEmail(email.CString(), password.CString(), "", createIfNotExisting,vars, successCallback, errorCallback);
}

void NakamaClient::AddLeaderboardScore(String leaderboardId,int64_t score)
{
    auto successCallback = [this](const NLeaderboardRecord& record)
    {
      //std::cout << "New record with score " << record.score << std::endl;
        URHO3D_LOGINFOF("new record with score %i (%s)",record.score,record.ownerId.c_str());
    };
    auto errorCallback = [](const NError& error)
    {
        URHO3D_LOGINFOF("nakama: error %s",error.message.c_str());
    };
    client_->writeLeaderboardRecord(session_,leaderboardId.CString(),score,opt::nullopt,opt::nullopt,successCallback,errorCallback);
}


bool NakamaClient::IsRTClientConnected()
{
    return rtClient_->isConnected();
}


void NakamaClient::AddToMatchmaker(int minCount,int maxCount,String query)
{
    if (!matchmakerTicket_.Empty()) return; // already matchmaking
    if (!rtClient_->isConnected()) return;

    auto successCallback = [this](const NMatchmakerTicket& ticket)
    {
        matchmakerTicket_ = ticket.ticket.c_str();
        URHO3D_LOGINFOF("nakama: matchedTicket %s",ticket.ticket.c_str());
    };

    auto errorCallback = [](const NRtError& error)
    {
        URHO3D_LOGINFOF("nakama: error %s",error.message.c_str());
    };

    NStringMap stringProperties;
    NStringDoubleMap numericProperties;

    //stringProperties.emplace("region", "europe");
    //numericProperties.emplace("rank", 8.0);

    rtClient_->addMatchmaker(
        minCount,
        maxCount,
        query.CString(),
        stringProperties,
        numericProperties,
        successCallback,
        errorCallback);

}

void NakamaClient::RemoveFromMatchmaker()
{
    if (matchmakerTicket_.Empty()) return;

    auto successCallback = [this]()
    {
        matchmakerTicket_ = "";
        URHO3D_LOGINFOF("nakama: removed matchmakerticket");
    };

    auto errorCallback = [](const NRtError& error)
    {
        URHO3D_LOGINFOF("nakama: error %s",error.message.c_str());
    };


    rtClient_->removeMatchmaker(std::string(matchmakerTicket_.CString())
                                ,successCallback
                                ,errorCallback);
}

void NakamaClient::RequestLeaderboard(String leaderboardId, int amount)
{
    auto successCallback = [this,&leaderboardId](NLeaderboardRecordListPtr recordsList)
    {
        using namespace NakamaLeaderboardResult;
        VariantMap map;
        map[P_LEADERBOARDID]=leaderboardId;
        map[P_RECORDS]=MakeCustomValue(recordsList->records);
        map[P_OWNERRECORDS]=MakeCustomValue(recordsList->ownerRecords);
        map[P_NEXTCURSOR]=String(recordsList->nextCursor.c_str());
        map[P_PREVCURSOR]=String(recordsList->prevCursor.c_str());

        SendEvent(E_NAKAMA_LEADERBOARD_RESULT,map);
    };

    auto errorCallback = [](const NError& error)
    {
        URHO3D_LOGINFOF("nakama: error %s",error.message.c_str());
    };


    client_->listLeaderboardRecordsAroundOwner(session_,leaderboardId.CString(),SessionUserId().CString(),amount,successCallback,errorCallback);
    client_->listLeaderboardRecords(session_,
      leaderboardId.CString(),
      {},
      amount,
      opt::nullopt,
      successCallback,
      errorCallback
    );
}

void NakamaClient::HandleBeginFrame(StringHash eventType, VariantMap& eventData)
{
    client_->tick();
    if (rtClient_) rtClient_->tick();
}
