#pragma once
// Private C ABI boundary shared by the adapters, not an installed public header.
#include "EncryptionPortable.h"
#include "../EncryptionCore.h"
#include <new>

namespace encryption::c_api {
struct CallbackError { EncryptionResult result; };
inline void callback_result(EncryptionResult result) {
    if (result != ENCRYPTION_OK) throw CallbackError{result};
}
template<class F> EncryptionResult checked(F&& call) noexcept {
    try { call(); return ENCRYPTION_OK; }
    catch (const CallbackError& error) { return error.result; }
    catch (const std::bad_alloc&) { return ENCRYPTION_OUT_OF_MEMORY; }
    catch (const std::length_error&) { return ENCRYPTION_OUT_OF_MEMORY; }
    catch (const Error& error) {
        switch (error.code) {
        case ErrorCode::argument: return ENCRYPTION_INVALID_ARGUMENT;
        case ErrorCode::state: return ENCRYPTION_INVALID_STATE;
        case ErrorCode::padding: return ENCRYPTION_INVALID_PADDING;
        case ErrorCode::io: return ENCRYPTION_IO_ERROR;
        default: return ENCRYPTION_CRYPTO_ERROR;
        }
    } catch (...) { return ENCRYPTION_CRYPTO_ERROR; }
}
inline KeyIv material(const uint8_t* key, const uint8_t* iv) {
    KeyIv result{};
    std::copy_n(key, 16, result.key.begin());
    std::copy_n(iv, 16, result.iv.begin());
    return result;
}
} // namespace encryption::c_api
