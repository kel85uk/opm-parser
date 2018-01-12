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

#include <opm/parser/eclipse/EclipseState/Aquancon.hpp>
#include <algorithm>
#include <iterator>

namespace Opm {

    Aquancon::Aquancon(const EclipseGrid& grid, const Deck& deck)
    { 
        if (!deck.hasKeyword("AQUANCON"))
        {
            std::cout<<("The analytical aquifer connections must be specified in the deck with the AQUANCON keyword")<<std::endl;
            // return an empty m_aquoutput vector
        }

        const auto& aquanconKeyword = deck.getKeyword("AQUANCON");
        // Resize the parameter vector container based on row entries in aquancon
        m_aqurecord.resize(aquanconKeyword.size());
        m_aquiferID_per_record.resize(aquanconKeyword.size());

        // We now do a loop over each record entry in aquancon

        for (size_t aquanconRecordIdx = 0; aquanconRecordIdx < aquanconKeyword.size(); ++ aquanconRecordIdx) 
        {
            const auto& aquanconRecord = aquanconKeyword.getRecord(aquanconRecordIdx);
            m_aquiferID_per_record.at(aquanconRecordIdx) = aquanconRecord.getItem("AQUIFER_ID").template get<int>(0);

            m_aqurecord.at(aquanconRecordIdx).i1 = aquanconRecord.getItem("I1").template get<int>(0);
            m_aqurecord.at(aquanconRecordIdx).i2 = aquanconRecord.getItem("I2").template get<int>(0);
            m_aqurecord.at(aquanconRecordIdx).j1 = aquanconRecord.getItem("J1").template get<int>(0);
            m_aqurecord.at(aquanconRecordIdx).j2 = aquanconRecord.getItem("J2").template get<int>(0);
            m_aqurecord.at(aquanconRecordIdx).k1 = aquanconRecord.getItem("K1").template get<int>(0);
            m_aqurecord.at(aquanconRecordIdx).k2 = aquanconRecord.getItem("K2").template get<int>(0);

            m_aquiferID_per_record.at(aquanconRecordIdx) = aquanconRecord.getItem("AQUIFER_ID").template get<int>(0);
            m_maxAquID = (m_maxAquID < m_aquiferID_per_record.at(aquanconRecordIdx) )?
                            m_aquiferID_per_record.at(aquanconRecordIdx) : m_maxAquID;
            
            m_aqurecord.at(aquanconRecordIdx).influx_coeff_per_record = 
                                              aquanconRecord.getItem("INFLUX_COEFF").getSIDouble(0);

            m_aqurecord.at(aquanconRecordIdx).influx_mult_per_record = 
                                              aquanconRecord.getItem("INFLUX_MULT").getSIDouble(0);

            m_aqurecord.at(aquanconRecordIdx).face_per_record = 
                                              aquanconRecord.getItem("FACE").getTrimmedString(0);
            
            // Loop over the cartesian indices to convert to the global grid index
            for (int k=m_aqurecord.at(aquanconRecordIdx).k1; k <= m_aqurecord.at(aquanconRecordIdx).k2; k++) {
                for (int j=m_aqurecord.at(aquanconRecordIdx).j1; j <= m_aqurecord.at(aquanconRecordIdx).j2; j++)
                    for (int i=m_aqurecord.at(aquanconRecordIdx).i1; i <= m_aqurecord.at(aquanconRecordIdx).i2; i++)
                        m_aqurecord.at(aquanconRecordIdx).global_index_per_record.
                                                              push_back( 
                                                                grid.getGlobalIndex(i-1 ,j-1 ,k- 1)
                                                              );
            } // outer-most for loop for index k = [k1,k2]
        }

        // Collate_function
        collate_function(m_aquoutput);

        // Logic applied here within our own private function
        logic_application(m_aquoutput);

    }                                          
    
    void Aquancon::collate_function(std::vector<Aquancon::AquanconOutput>& output_vector)
    {
        output_vector.resize(m_maxAquID);
        for (auto it = m_aquiferID_per_record.begin(); it != m_aquiferID_per_record.end(); ++it)
                std::cout << *it << std::endl;
        // Find record indices at which the aquifer ids are located in
        for (int i = 1; i <= m_maxAquID; ++i)
        {
            std::vector<int> result_id;

            convert_record_id_to_aquifer_id(result_id, i);

            std::cout << "Aquifer ID = " << i << ": Result_id = " << std::endl;
            for (auto it = result_id.begin(); it != result_id.end(); ++it)
                std::cout << *it << std::endl;

            // We add the aquifer id into each element of output_vector
            output_vector.at(i - 1).aquiferID = i;
            for ( auto record_index_matching_id : result_id)
            {
                output_vector.at(i - 1).global_index.insert(
                                                            output_vector.at(i - 1).global_index.end(),
                                                            m_aqurecord.at(record_index_matching_id).global_index_per_record.begin(),
                                                            m_aqurecord.at(record_index_matching_id).global_index_per_record.end()
                                                          );
            }
        }
        // Then with the container of the unique IDs, append the corresponding global indices, etc...
    }

    void Aquancon::logic_application(std::vector<Aquancon::AquanconOutput>& output_vector)
    {
        // Are the aquifer IDs the same for each record?
        // m_aqurecord
    }

    void Aquancon::convert_record_id_to_aquifer_id(std::vector<int>& record_indices_matching_id, int i)
    {
        auto it = std::find_if( m_aquiferID_per_record.begin(), m_aquiferID_per_record.end(), 
                                    [&](int id) {
                                        return id == i;
                                    } 
                              );
        while (it != m_aquiferID_per_record.end()) {
            record_indices_matching_id.emplace_back(std::distance(m_aquiferID_per_record.begin(), it));
            it = std::find_if(std::next(it), m_aquiferID_per_record.end(), [&](int id){return id == i;});
        }
        
        // Resize the result container
        // result_id.resize( std::distance(result_id.begin(), it) );
    }

    const std::vector<Aquancon::AquanconOutput>& Aquancon::getAquOutput() const
    {
        return m_aquoutput;
    }

}


// Collate into unique aquifers
          // Within collate
            //TODO: find if Global index is repeated for each aquifer, if so, select only the first one
            //TODO: Find if face on outside of reservoir or adjoins an inactive cell 
            //TODO: Total number of grid blocks connected to aquifer must not exceed item 6 of AQUDIMS