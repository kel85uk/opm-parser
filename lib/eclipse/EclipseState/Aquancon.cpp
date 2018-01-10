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

#include <opm/parser/eclipse/EclipseState/AquiferCT.hpp>

namespace Opm {

    Aquancon::Aquancon(const EclipseGrid grid, const Deck& deck)
    { 
        if (!deck.hasKeyword("AQUANCON")){
            std::cout<<("The analytical aquifer connections must be specified in the deck with the AQUANCON keyword")<<std::endl;
            }

        const auto& aquanconKeyword = deck.getKeyword("AQUANCON");
        // Resize the parameter vector container based on row entries in aquancon
        m_aquancon.resize(aquanconKeyword.size());
        //For now assuming AQUANCON keyword defines connection only for one aquifer
        for (size_t aquanconRecordIdx = 0; aquanconRecordIdx < aquanconKeyword.size(); ++ aquanconRecordIdx) 
            {
            const auto& aquanconRecord = aquanconKeyword.getRecord(aquanconRecordIdx);

            m_aquancon.at(aquanconRecordIdx).i1 = aquanconRecord.getItem("I1").template get<int>(0);
            m_aquancon.at(aquanconRecordIdx).i2 = aquanconRecord.getItem("I2").template get<int>(0);
            m_aquancon.at(aquanconRecordIdx).j1 = aquanconRecord.getItem("J1").template get<int>(0);
            m_aquancon.at(aquanconRecordIdx).j2 = aquanconRecord.getItem("J2").template get<int>(0);
            m_aquancon.at(aquanconRecordIdx).k1 = aquanconRecord.getItem("K1").template get<int>(0);
            
            for (int k=m_aquancon.at(aquanconRecordIdx).k1; k <= m_aquancon.at(aquanconRecordIdx).k2; k++) {
                for (int j=m_aquancon.at(aquanconRecordIdx).j1; j <= m_aquancon.at(aquanconRecordIdx).j2; j++) {
                    for (int i=m_aquancon.at(aquanconRecordIdx).i1; i <= m_aquancon.at(aquanconRecordIdx).i2; i++) {

                        struct AquiferCell m_aqucell;
                        m_aqucell.aquiferID = aquanconRecord.getItem("AQUIFER_ID").template get<int>(0);
                        m_aqucell.global_index = grid.getGlobalIndex(i-1 ,j-1 ,k- 1);
                        m_aqucell.influx_coeff = aquanconRecord.getItem("INFLUX_COEFF").getSIDouble(0);
                        m_aqucell.influx_mult = aquanconRecord.getItem("INFLUX_MULT").getSIDouble(0);
                        m_aqucell.face = aquanconRecord.getItem("FACE").getTrimmedString(0);

                        //TODO: find if Global index is repeated for each aquifer, if so, select only the first one
                        //TODO: Find if face on outside of reservoir or adjoins an inactive cell 
                        //TODO: Total number of grid blocks connected to aquifer must not exceed item 6 of AQUDIMS
                    }
                }
            }
            
        }
    }                                          
        

    const std::vector<AquiferCT::AQUANCON_data>& Aquancon::getAquancon() const
    {
        return m_aquancon;
    }

    const std::vector<Aquancon::AquiferCell>& Aquancon::getConnection() const
    {
        return m_aqucell;
    }

}