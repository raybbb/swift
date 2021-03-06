/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/TLS/TLSContext.h>
#include <Swiften/TLS/TLSOptions.h>

namespace Swift {
    class SWIFTEN_API TLSContextFactory {
        public:
            virtual ~TLSContextFactory();

            virtual bool canCreate() const = 0;

            virtual TLSContext* createTLSContext(const TLSOptions& tlsOptions, TLSContext::Mode = TLSContext::Mode::Client) = 0;
            virtual void setCheckCertificateRevocation(bool b) = 0;
            virtual void setDisconnectOnCardRemoval(bool b) = 0;
    };
}
