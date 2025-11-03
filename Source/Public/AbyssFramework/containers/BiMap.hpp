#pragma once
#include "Log.h"
#include <map>
#include <iostream>
#include <type_traits>

namespace aby::containers {

    enum class EBiMapSide { Left, Right };

    template <typename K, typename V>
    concept CBiMap = !std::is_same_v<K, V> && std::is_default_constructible_v<K> && std::is_default_constructible_v<V>;

    template <typename K, typename V> requires(CBiMap<K, V>)
    class BiMap {
    public:
        using key_type = K;
        using mapped_type = V;
        using size_type = std::size_t;
        using iterator = typename std::map<K, V>::const_iterator;

        iterator begin() const { return m_Fwd.cbegin(); }
        iterator end() const { return m_Fwd.cend(); }

        bool insert(const K& k, const V& v) {
            if (m_Fwd.count(k) || m_Rev.count(v))
                return false;

            m_Fwd[k] = v;
            m_Rev[v] = k;
            return true;
        }

        template <typename KV>
        auto at(const KV& kv) const -> const auto& {
        #ifndef NDEBUG
            if constexpr (std::is_same_v<KV, K> || std::is_convertible_v<KV, K>) {
                if (!m_Fwd.count(kv)) {
                    log_err("BiMap::at: Key not found: {}", kv);
                    static V dv{};
                    return dv; // fallback value for missing key
                }
                return m_Fwd.at(kv);
            } else if constexpr (std::is_same_v<KV, V> || std::is_convertible_v<KV, V>) {
                if (!m_Rev.count(kv)) {
                    log_err("BiMap::at: Value not found: {}", kv);
                    static K dk{};
                    return dk; // fallback value for missing value
                }
                return m_Rev.at(kv);
            } else {
                static_assert(std::is_same_v<KV, K> || std::is_same_v<KV, V>, "Type must be K or V");
            }
        #else
            if constexpr (std::is_same_v<KV, K> || std::is_convertible_v<KV, K>)
                return m_Fwd.at(kv);
            else if constexpr (std::is_same_v<KV, V> || std::is_convertible_v<KV, V>)
                return m_Rev.at(kv);
        #endif
        }


        template <typename KV>
        bool contains(const KV& kv) const {
            if constexpr (std::is_same_v<KV, K> || std::is_convertible_v<KV, K>) {
                return m_Fwd.count(kv) > 0;
            } else if constexpr (std::is_same_v<KV, V> || std::is_convertible_v<KV, V>) {
                return m_Rev.count(kv) > 0;
            } else {
                static_assert(std::is_same_v<KV, K> || std::is_same_v<KV, V>, "Type must be K or V");
            }
        }

        template <typename KV>
        bool erase(const KV& kv) {
            if constexpr (std::is_same_v<KV, K> || std::is_convertible_v<KV, K>) {
                auto it = m_Fwd.find(kv);
                if (it == m_Fwd.end()) {
        #ifndef NDEBUG
                    log_warn("BiMap::erase: Key not found: {}", kv);
                    return false;
        #else
                    return false;
        #endif
                }
                m_Rev.erase(it->second);
                m_Fwd.erase(it);
                return true;
            } else if constexpr (std::is_same_v<KV, V> || std::is_convertible_v<KV, V>) {
                auto it = m_Rev.find(kv);
                if (it == m_Rev.end()) {
        #ifndef NDEBUG
                    log_warn("BiMap::erase: Value not found: {}", kv);
                    return false;
        #else
                    return false;
        #endif
                }
                m_Fwd.erase(it->second);
                m_Rev.erase(it);
                return true;
            } else {
                static_assert(std::is_same_v<KV, K> || std::is_same_v<KV, V>, "Type must be K or V");
            }
        }

        V& operator[](const K& k) {
            return m_Fwd[k];
        }

        K& operator[](const V& v) {
            return m_Rev[v];
        }

        void print(std::ostream& os = std::cout, EBiMapSide side = EBiMapSide::Left) const {
            os << "[\n    "; // opening bracket with initial indent
            int count = 0;

            if (side == EBiMapSide::Left) {
                for (const auto& [k, v] : m_Fwd) {
                    if (count > 0) os << ", ";
                    os << "(" << k << ", " << v << ")";
                    ++count;
                    if (count % 8 == 0) os << "\n    "; // newline + indent after 8 pairs
                }
            } else { // Right
                for (const auto& [v, k] : m_Rev) {
                    if (count > 0) os << ", ";
                    os << "(" << v << ", " << k << ")";
                    ++count;
                    if (count % 8 == 0) os << "\n    ";
                }
            }

            os << "\n]\n"; // final newline after closing bracket
        }


        size_type size() const { return m_Fwd.size(); }
        bool empty() const { return m_Fwd.empty(); }
        void clear() { m_Fwd.clear(); m_Rev.clear(); }

        const std::map<K, V>& left_map() const { return m_Fwd; }
        const std::map<V, K>& right_map() const { return m_Rev; }

    private:
        std::map<K, V> m_Fwd;
        std::map<V, K> m_Rev;
    };
}
