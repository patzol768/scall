# Make your calls secure

Some APIs are protected by mutual TLS and even with http signature. These are important security measures, but many of the available API testing tools simply does not support them.

This tool intends to take place in between the testing application (e.g. [hoppscotch](https://hoppscotch.io/)) and the secured service. While the traffic travels through, it adds http signature and initiates an mTLS call to the original service.

> [!CAUTION]
> The tools was never meant to be production ready. This is solely for internal testing purposes. Crypto Keys are unprotected.

# How to use

Here's a sample call I've used in my environment:

```
curl -k --request GET \
  --url https://localhost:7676/psd2/v1/consents/CO050050 \
    --header 'Accept: application/json' \
    --header 'Accept-Language: hu' \
    --header 'Connection: keep-alive' \
    --header 'Content-Type: application/json' \
    --header 'Date: Fri, 07 Jan 2024 16:46:21 GMT' \
    --header 'PSU-GEO-Location: GEO:47.49484;19.01234' \
    --header 'PSU-ID: usr0584521' \
    --header 'PSU-IP-Address: 192.168.1.1' \
    --header 'PSU-User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; rv:54.0) Gecko/20100101 Firefox/54.0' \
    --header 'X-Request-ID: 4d52a360-abd4-11ee-b8ac-f77c4f3c69c0'  \
    --header 'x-hs-algorithm: rsa-sha256' \
    --header 'x-hs-headers: date x-request-id psu-id digest' \
    --header 'x-hs-key-alias: psp' \
    --header 'x-hs-key-id: SN=864B06177B7C64AD,CA=CN=test_CA,O=TESTING,L=DEV,C=HU' \
    --header 'x-mtls-key-id: psp_qwac' \
    --header 'x-hs-send-certificate: TPP-Signature-Certificate'
```

As seen, the curl call itself has nothing to do with http signature and/or mTLS. Instead the last 6 parameters instruct the tool to do them.

The target server receives something like this. (Although the mTLS not seen here, the http signature headers are there. Necessary keys and certificates are stored on the scall server, as described in the configuration.)

```
GET /psd2/v1/consents/CO050050 {consentId}=CO050050
Accept: application/json
Accept-Language: hu
Connection: keep-alive
Content-Type: application/json
Date: Fri, 07 Jan 2024 16:46:21 GMT
Digest: SHA-256=47DEQpj8HBSa+/TImW+5JCeuQeRkm5NMpJWZG3hSuFU=
Host: localhost:7676
PSU-GEO-Location: GEO:47.47410;19.07012
PSU-ID: usr0584521
PSU-IP-Address: 192.168.1.1
PSU-User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; rv:54.0) Gecko/20100101 Firefox/54.0
Signature: keyId="SN=864B06177B7C64AD,CA=CN=test_CA,O=ESL,L=EBH,C=HU",headers="date x-request-id psu-id digest",algorithm="rsa-sha256",signature="Ny7lPjggvVO+6P2eTDAShA112IuTzSguZGh2NHiovWEj4UvjxGTkoI2Wq0Xi1YXqMnoAkar1RHR0TUT/tXo8Y+2+pWSdEbwc7jNztVBGJOGmEPQcBnN1aUZyxVbW3OmRPt7FrquZgv8YiMGA2R+XQ4EqbpDrtISoWq/yYsO6Z6xZuM8AueN32rSHt84YEU/vMQpPhHsn5gV7qfGViSboi/BY5UWqoNkmHvw9ghY603pHpiU1pV6pvdl3XQ7tWDIQNcucZakuIEh6ruDofFmxKiUDRs4NbfRokA9T+Kew6bwTSW7NiyuN3HySkgbUPJLaoGAbMTe+RfwiaEZW+k/F8Q=="
TPP-Signature-Certificate: ***removed***
User-Agent: curl/8.2.1
X-Request-ID: 4d52a360-abd4-11ee-b8ac-f77c4f3c69c0
```

# Build

The tool uses the cmake build system, one should use the usual commands. Some dependencies are downloaded during cmake configuration.

```
$ mkdir build
$ cd build
$ cmake .
$ cmake --install . --prefix /var/opt/scall
```

# Dependencies

- [restbed](https://github.com/Corvusoft/restbed)
- [restclient](https://github.com/mrtazz/restclient-cpp) - modified version, hence included in the source tree
- [nlohmann json](https://github.com/nlohmann/json)
- [catch2](https://github.com/catchorg/Catch2)

All the dependencies use their own licenses. I have no intention to break them at any point. In case you think there is still some issue, please connect in order to sort out peacefully.

# Configuration

The **config.json** contains the service configuration. Here below the comments are for clarity only. The configuration file is a pure JSON, hence does not support comments.

```
{
  // service endpoint to bind on http and https respectively
  "http": {
    "enabled": false,
    "address": "0.0.0.0",
    "port": 7675
  },

  "https": {
    "enabled": true,
    "address": "0.0.0.0",
    "port": 7676,

    // own certificate and key (watch on the file:// prefix)
    "certificate": "file://./servercert/scall_cert.pem",
    "key": "file://./servercert/scall_key.pem",

    // some more openssl related settings
    "sslv2_enabled": false,
    "sslv3_enabled": false,
    "tlsv1_enabled": false,
    "tlsv11_enabled": false,
    "tlsv12_enabled": true,
    "default_workarounds_enabled": true,
    "single_diffie_hellman_use_enabled": true,

    "compression_enabled": false
  },

  // number of restbed workers
  "worker_limit": 2,

  // maximum number of parallel open connections
  "connection_limit": 50,

  // connection timeout in seconds
  "connection_timeout": 10,

  // http verbs to forward
  "verbs": [
    "GET", "POST", "PUT", "PATCH", "DELETE", "OPTIONS"
  ],

  // since restbed tries to match the path, the tool mocks that we have a full tree with certain maximum depth
  "m_path_max_depth": 5,

  "target": {
    // target base Url
    "base_url": "https://localhost:1984",

    // whether or not to verify the target server's certificate
    "target_verify_peer": true,

    // certificate to use for validation of the target's certificate
    "ca": "file://./servercert/cacert.pem"
  },

  "keys": {
    // mTLS client certificate store directory
    //  - both certs and keys are stored here
    //  - certs have to be in PEM format with .pem extension -- *certs not needed currently*
    //  - keys have to be in PEM format with .key extension
    //  - the filename (without the extension is the key identifier)
    "dir": "./clientcert/",

    // one can define passwords for each key (if not defined, the tool assumes no password)
    // the key here is the key file's name, without the extension
    "passwords": {
      "key_id_1": "somepass",
      "key_id_2": "anotherpass"
    }
  },

  "http_signature": {
    // if true, the tool would add http signature to the outgoing call
    "enabled": true,

    // http signature standard not yet finalized, hence multiple formats are on the market
    // the "version" selects the draft to use.
    //  - "cavage12": https://datatracker.ietf.org/doc/html/draft-cavage-http-signatures-12 (partially supported)
    //  - "httpbis19": https://datatracker.ietf.org/doc/html/draft-ietf-httpbis-message-signatures (not yet supported)
    "version": "cavage12",

    // to allow flexibility, the algorithm parameters would be read from incoming http headers
    // the configuration parameters mostly define which header to look for a specific data
    //
    // these headers are *not* forwarded to the target service 
    
    "cavage12_params": {
      // http signature contains sub-fields:
      //  - keyId: contains the key reference in a format understandable by the target service
      //  - algorithm: contains the signature algorithm name (also the digest algorithm is extracted from here)
      //  - headers: lists those headers which are to be signed, in the order of their signature
      //  - signature: this is to be computed by the tool
      "key_id": "x-hs-key-id",
      "algorithm": "x-hs-algorithm",
      "headers": "x-hs-headers",

      // some target services may require rather strange keyId structure (e.g. these might contain
      // such characters, what might be not supported on the file store)
      //
      // if the caller defines a key alias, than the key and certificate would be looked up with this
      // key, instead of the target key id
      //
      // if this header is missing, the key lookup happens with the target key id value (see above)
      "key_id_alias": "x-hs-key-alias",

      // if this header presents, then the tool would send the certificate in the referred header
      "send_certificate": "x-hs-send-certificate"
    }
  },

  "mtls": {
    // if true, the tool would use mTLS to connect the target service
    "enabled": "true",

    // the caller should use this http header to refer to the key and certificate to be used as the
    // mtls client identificator
    "key_id": "x-mtls-key-id"
  }

}
```

# References

* [draft http signature "cavage"](https://datatracker.ietf.org/doc/html/draft-cavage-http-signatures-12)
* [draft http signature "httpbis"](https://datatracker.ietf.org/doc/html/draft-ietf-httpbis-message-signatures)

# TODO

Definitely there are a lot of possible improvements. Like:
* some unit tests
* some more logging
* a gui for configuration and maybe for request / response lookup
* support for tlsv13
* allow more configuration for curl / openssl / restbed
* etc.
