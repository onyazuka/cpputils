#include <openssl/sha.h>
#include <string>

namespace util::crypto {
	std::string sha256(const std::string& str);
}