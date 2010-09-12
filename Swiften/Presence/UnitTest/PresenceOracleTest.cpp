/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Presence/PresenceOracle.h"
#include "Swiften/Client/DummyStanzaChannel.h"

using namespace Swift;

class PresenceOracleTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(PresenceOracleTest);
		CPPUNIT_TEST(testReceivePresence);
		CPPUNIT_TEST(testReceivePresenceFromDifferentResources);
		CPPUNIT_TEST(testSubscriptionRequest);
		CPPUNIT_TEST(testReconnectResetsPresences);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			stanzaChannel_ = new DummyStanzaChannel();
			oracle_ = new PresenceOracle(stanzaChannel_);
			oracle_->onPresenceChange.connect(boost::bind(&PresenceOracleTest::handlePresenceChange, this, _1));
			oracle_->onPresenceSubscriptionRequest.connect(boost::bind(&PresenceOracleTest::handlePresenceSubscriptionRequest, this, _1, _2));
			user1 = JID("user1@foo.com/Foo");
			user1alt = JID("user1@foo.com/Bar");
			user2 = JID("user2@bar.com/Bar");
		}

		void tearDown() {
			delete oracle_;
			delete stanzaChannel_;
		}

		void testReceivePresence() {
			boost::shared_ptr<Presence> sentPresence(createPresence(user1));
			stanzaChannel_->onPresenceReceived(sentPresence);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(subscriptionRequests.size()));
			CPPUNIT_ASSERT_EQUAL(sentPresence, changes[0]);
			CPPUNIT_ASSERT_EQUAL(sentPresence, oracle_->getLastPresence(user1));
		}

		void testReceivePresenceFromDifferentResources() {
			boost::shared_ptr<Presence> sentPresence1(createPresence(user1));
			boost::shared_ptr<Presence> sentPresence2(createPresence(user1alt));
			stanzaChannel_->onPresenceReceived(sentPresence1);
			stanzaChannel_->onPresenceReceived(sentPresence2);

			CPPUNIT_ASSERT_EQUAL(sentPresence1, oracle_->getLastPresence(user1));
			CPPUNIT_ASSERT_EQUAL(sentPresence2, oracle_->getLastPresence(user1alt));
		}
		
		void testSubscriptionRequest() {
			String reasonText = "Because I want to";
			JID sentJID = JID("me@example.com");

			boost::shared_ptr<Presence> sentPresence(new Presence());
			sentPresence->setType(Presence::Subscribe);
			sentPresence->setFrom(sentJID);
			sentPresence->setStatus(reasonText);
			stanzaChannel_->onPresenceReceived(sentPresence);

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(subscriptionRequests.size()));
			CPPUNIT_ASSERT_EQUAL(sentJID, subscriptionRequests[0].jid);
			CPPUNIT_ASSERT_EQUAL(reasonText, subscriptionRequests[0].reason);
		}

		void testReconnectResetsPresences() {
			boost::shared_ptr<Presence> sentPresence(createPresence(user1));
			stanzaChannel_->onPresenceReceived(sentPresence);
			stanzaChannel_->setAvailable(false);
			stanzaChannel_->setAvailable(true);

			CPPUNIT_ASSERT(!oracle_->getLastPresence(user1));
		}
	
	private:
		void handlePresenceChange(boost::shared_ptr<Presence> newPresence) {
			changes.push_back(newPresence);
		}
		
		void handlePresenceSubscriptionRequest(const JID& jid, const String& reason) {
			SubscriptionRequestInfo subscriptionRequest;
			subscriptionRequest.jid = jid;
			subscriptionRequest.reason = reason;
			subscriptionRequests.push_back(subscriptionRequest);
		}

		boost::shared_ptr<Presence> createPresence(const JID& jid) {
			boost::shared_ptr<Presence> sentPresence(new Presence("blarb"));
			sentPresence->setFrom(jid);
			return sentPresence;
		}

	private:
		struct SubscriptionRequestInfo {
			JID jid;
			String reason;
		};
		PresenceOracle* oracle_;
		DummyStanzaChannel* stanzaChannel_;
		std::vector<Presence::ref> changes;
		std::vector<SubscriptionRequestInfo> subscriptionRequests;
		JID user1;
		JID user1alt;
		JID user2;
};

CPPUNIT_TEST_SUITE_REGISTRATION(PresenceOracleTest);

