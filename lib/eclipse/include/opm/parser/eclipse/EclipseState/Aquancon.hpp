/*
  Copyright (C) 2017 TNO

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

#ifndef OPM_AQUANCON_HPP
#define OPM_AQUANCON_HPP

/*
  The Aquancon is a data container object meant to hold the data from the AQUANCON keyword.
  This includes the logic for parsing as well as the associated tables. It is meant to be used by opm-grid and opm-simulators in order to
  implement the Carter Tracy analytical aquifer model in OPM Flow.
*/

#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/Parser/ParserKeywords/A.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/Deck/DeckItem.hpp>
#include <opm/parser/eclipse/Deck/DeckRecord.hpp>
#include <opm/parser/eclipse/Deck/DeckKeyword.hpp>
#include <opm/parser/eclipse/EclipseState/Tables/Aqudims.hpp>
#include <opm/parser/eclipse/EclipseState/Tables/TableContainer.hpp>
#include <opm/parser/eclipse/EclipseState/Tables/AqutabTable.hpp>
#include <boost/concept_check.hpp>

namespace Opm {

    class Aquancon {
        public:

            struct AQUANCON_data{

                    // Grid cell box definition to connect aquifer
                    int i1, i2, j1, j2, k1, k2;

            };

            struct AquiferCell{

                    size_t global_index;
                    // Aquifer ID
                    int aquiferID;
                    // Variables constants
                    double  influx_coeff,  //Aquifer influx coefficient
                            influx_mult;   //Aquifer influx coefficient Multiplier       
                    // Cell face to connect aquifer to        
                    std::string face;           

            };

            Aquancon(const EclipseGrid grid, const Deck& deck);

            const std::vector<Aquancon::AQUANCON_data>& getAquancon() const;
            const std::vector<Aquancon::AquiferCell>& getConnection() const;
    
        private:
  
            std::vector<Aquancon::AQUANCON_data> m_aquancon;
            std::vector<Aquancon::AquiferCell> m_aqucell;

    };
}


#endif
