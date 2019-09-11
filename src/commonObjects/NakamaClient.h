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

#pragma once

#include <Urho3D/Urho3DAll.h>

#include <nakama-cpp/Nakama.h>
#include "nakama-cpp/realtime/NRtDefaultClientListener.h"


using namespace Urho3D;
using namespace Nakama;

class NakamaClient : public Object
{
    URHO3D_OBJECT(NakamaClient, Object);

public:
    /// Construct.
    explicit NakamaClient(Context* context);

    virtual ~NakamaClient(){
        int a=0;
    }

    /// Register object factory and attributes.
    static void RegisterObject(Context* context);

    void Connect(const String& host="192.168.178.110",int port=DEFAULT_PORT,const String& serverKey="defaultkey");
    void Authenticate(const String& email,const String& password, bool createIfNotExisting=false);

    inline bool HasSession() { return session_!=0; }
    inline bool SessionIsValid() { return !session_->isExpired(); }
    inline NTimestamp SessionExpireTime() { return session_->getExpireTime(); }
    inline String SessionUserId() { return session_->getUserId().c_str(); }
    inline String SessionUserName() { return session_->getUsername().c_str(); }
    inline bool IsAuthenticated() { return authenticated_; }
    bool IsRTClientConnected();
    inline const String& GetMatchmakingTicket() { return matchmakerTicket_; }

    void AddLeaderboardScore(String leaderboardId,int64_t score);
    void RequestLeaderboard(String leaderboardId,int amount=10);

    void AddToMatchmaker(int minCount,int maxCount,String query);
    void RemoveFromMatchmaker();
    void HandleBeginFrame(StringHash eventType, VariantMap& eventData);

private:
    NClientPtr client_;
    NSessionPtr session_;
    bool authenticated_;
    NRtDefaultClientListener listener_;
    NRtClientPtr rtClient_;
    String matchmakerTicket_;

};

