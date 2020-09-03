//		
//	pwCollection
//	File: 
//		pwTextParser.hpp
//
//	Description:
//		Parses formatted (format see below) text files into key => value pairs.
//
//	Version:
//		1.0
//
//	Usage:
//		Given a text file with contents:
//
//			tag1			"value1"
//			tag2			10
//			tag3			1.5
//
//			container1 {
//				subtag1			"subvalue1"
//				subtag2			10
//				subtag3			1.5
//	
//				subcontainer1 {
//					subsubtag1			"subsubvalue1"
//
//					[...]
//				}
//			}
//
//		This helper class will parse it into a key value pair:
//
//			tag1 => value1
//			tag2 => 10
//			tag3 => 1.5
//			container1/subtag1 => subvalue1
//			container1/subtag2 => 10
//			container1/subtag3 => 1.5
//			container1/subcontainer1/subtag1 => subsubvalue1
//			[...]
//
//		You get the point.
//		Comments are not supported.		
//
//		As all data is stored as string values the helper class also comes with conversion functions.
//
#ifndef __PW__TEXTPARSER__HPP
#define __PW__TEXTPARSER__HPP

#include <string>
#include <sstream>
#include <map>

#ifdef _UNICODE
	#ifndef pwTEXT
		#define pwTEXT(x)		L ## x
	#endif
	#ifndef pwToString
		#define pwToString(x) std::to_wstring((x))
	#endif
#else
	#ifndef pwTEXT
		#define pwTEXT(x)		x
	#endif
	#ifndef pwToString
		#define pwToString(x) std::to_string((x))
	#endif
#endif

namespace pw {
	class TextParser {
	public:

#ifdef _UNICODE
		using string_t			= std::wstring;
		using stringstream_t	= std::wstringstream;
		using ostringstream_t	= std::wostringstream;
		using char_t			= wchar_t;
#else
		using string_t			= std::string;
		using stringstream_t	= std::stringstream;
		using ostringstream_t	= std::ostringstream;
		using char_t			= char;		
#endif
		
		using ValueMap = std::map<string_t, string_t>;

	private:
		string_t			m_Data;
		char_t				m_Delim;
		ValueMap			m_Values;

	public:
		inline TextParser()
			: m_Data()
			, m_Delim(pwTEXT('\0'))
			, m_Values()
		{
		}

		inline TextParser(const string_t& sText, char_t delim = pwTEXT('\0'))
			: m_Data(sText)
			, m_Delim(delim)
			, m_Values()
		{
			TextParser::Parse(m_Data, m_Values, delim);
		}

		inline TextParser(const TextParser& p)
			: m_Data(p.m_Data)
			, m_Delim(p.m_Delim)
			, m_Values(p.m_Values)
		{
		}

		inline TextParser(TextParser&& p) noexcept
			: m_Data(std::move(p.m_Data))
			, m_Delim((char_t&&)(p.m_Delim))
			, m_Values(std::move(p.m_Values))
		{
		}

		inline TextParser& operator=(const TextParser& p) {
			m_Data = p.m_Data;
			m_Delim = p.m_Delim;
			m_Values = p.m_Values;
			return *this;
		}

		inline TextParser& operator=(TextParser&& p) noexcept {
			m_Data = std::move(p.m_Data);
			m_Delim = (char_t&&)(p.m_Delim);
			m_Values = std::move(p.m_Values);
			return *this;
		}

		inline string_t operator[](const string_t& sKey) const {
			if (m_Values.count(sKey) == 0) {
				return pwTEXT("");
			}
			return m_Values.at(sKey);
		}

		inline string_t& operator[](const string_t& sKey) {
			return m_Values[sKey];
		}

		inline operator bool() const {
			return (!m_Data.empty() && m_Values.size() != 0);
		}

		inline ValueMap::iterator begin() { return m_Values.begin(); }
		inline ValueMap::const_iterator begin() const { return m_Values.begin(); }
		inline ValueMap::iterator end() { return m_Values.end(); }
		inline ValueMap::const_iterator end() const { return m_Values.end(); }
		inline ValueMap::reverse_iterator rbegin() { return m_Values.rbegin(); }
		inline ValueMap::const_reverse_iterator	rbegin() const { return m_Values.rbegin(); }
		inline ValueMap::reverse_iterator rend() { return m_Values.rend(); }
		inline ValueMap::const_reverse_iterator	rend() const { return m_Values.rend(); }
		inline ValueMap::const_iterator cbegin() const { return m_Values.cbegin(); }
		inline ValueMap::const_iterator cend() const { return m_Values.cend(); }

		inline bool Contains(const string_t& sKey) const {
			return (m_Values.count(sKey) > 0);
		}

		inline size_t Size(void) const {
			return m_Values.size();
		}		

		template<typename T>
		inline void AddValue(const string_t& sKey, const T& t) {
			m_Values[sKey] = pwToString(t);
		}

		template<>
		inline void AddValue<string_t>(const string_t& sKey, const string_t& t) {
			m_Values[sKey] = t;
		}

		inline void AddValue(const string_t& sKey, const char_t* t) {
			m_Values[sKey] = t;
		}

		template<typename T>
		inline bool GetValue(const string_t& sKey, T& t) const {
			if (!Contains(sKey)) {
				return false;
			}

			stringstream_t ss(m_Values.at(sKey));
			ss >> t;
			return (bool)(ss);
		}

		template<>
		inline bool GetValue<string_t>(const string_t& sKey, string_t& s) const {
			if (!Contains(sKey)) {
				return false;
			}
			s = m_Values.at(sKey);
			return true;
		}

		template<typename T>
		inline T GetValue(const string_t& sKey) const {
			if (!Contains(sKey)) { return (T)(0); }		// (T)(0) => evil >:D

			T r;
			stringstream_t ss(m_Values.at(sKey));
			if (!(ss >> r)) { return (T)(0); }

			return r;
		}

		template<>
		inline string_t GetValue(const string_t& sKey) const {
			if (!Contains(sKey)) { return pwTEXT(""); }
			return m_Values.at(sKey);
		}

		inline static void Parse(const string_t& sData, ValueMap& list, char_t delim = pwTEXT('\0')) {
			auto stepDown = [](string_t& sPath) {
				size_t last = sPath.rfind(pwTEXT('/'));
				if (last != string_t::npos) {
					sPath = sPath.substr(0, last);
				}
				else {
					sPath = pwTEXT("");
				}
			};

			auto stepUp = [](string_t& sPath, const string_t& s) {
				if (sPath.empty()) {
					sPath = s;
				}
				else if (!s.empty()) {
					sPath += pwTEXT('/');
					sPath += s;
				}
			};

			list.clear();

			ostringstream_t oss;
			string_t sCurrent = pwTEXT("");
			string_t sValue = pwTEXT("");
			bool bString = false;

			for (auto& c : sData) {
				if (c == pwTEXT(';')) {
					sValue = oss.str();

					if (sValue.front() == pwTEXT('\"')) {
						if (sValue.back() == pwTEXT('\"') && sValue.size() > 2) {
							list[sCurrent].assign(sValue.begin() + 1, sValue.end() - 1);
						}
						else {
							list[sCurrent] = pwTEXT("");
						}
					}
					else {
						list[sCurrent] = sValue;
					}

					bString = false;

					oss.str(pwTEXT(""));
					stepDown(sCurrent);
					continue;
				}
				else if (c == pwTEXT('}')) {
					stepDown(sCurrent);
					oss.str(pwTEXT(""));
					continue;
				}
				else if (c == pwTEXT('{')) {
					stepUp(sCurrent, oss.str());
					oss.str(pwTEXT(""));
					continue;
				}
				else if (c == pwTEXT('\"')) {
					bString = true;
				}
				else {
					if (delim != pwTEXT('\0')) {
						if (c == delim) {
							stepUp(sCurrent, oss.str());
							oss.str(pwTEXT(""));
							continue;
						}
						else if (std::isspace(c) && !bString) continue;
					}
					else {
						if (std::isspace(c) && !bString) {
							stepUp(sCurrent, oss.str());
							oss.str(pwTEXT(""));
							continue;
						}
					}
				}

				oss << c;
			}
		}
	};
}

#endif
