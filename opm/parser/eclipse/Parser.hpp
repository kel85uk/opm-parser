/*
  Copyright 2013 Statoil ASA.

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PARSER_H
#define	PARSER_H
#include <string>
#include <fstream>

#include "Logger.hpp"
#include "data/KeywordRecordSet.hpp"

namespace Opm {

    class Parser {
    public:
        Parser();
        Parser(const std::string &path);
        void parse();
        void parse(const std::string &path);
        int getNumberOfKeywords();
        void getListOfKeywords(std::list<std::string>& list);
        virtual ~Parser();
    private:
        std::string m_dataFilePath;
        Logger m_logger;
        std::list<KeywordRecordSet> m_keywordRawDatas;
        void initInputStream(const std::string &path, std::ifstream& file);
        void readKeywordAndDataTokens(std::ifstream& inputstream);
        bool isKeyword(const std::string& line);
        void addDataToDataToken(const std::string& line, KeywordRecordSet& currentKeywordRecordSet);
        bool looksLikeData(const std::string& line);
        void checkInputFile(const std::string& pathToInputFile);
        
    };
} // namespace Opm
#endif	/* PARSER_H */
