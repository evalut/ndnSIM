/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014  Regents of the University of California,
 *                     Arizona Board of Regents,
 *                     Colorado State University,
 *                     University Pierre & Marie Curie, Sorbonne University,
 *                     Washington University in St. Louis,
 *                     Beijing Institute of Technology,
 *                     The University of Memphis
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include "rib.hpp"

namespace nfd {
namespace rib {

Rib::Rib()
{
}


Rib::~Rib()
{
}

static inline bool
compareNameFaceProtocol(const RibEntry& entry1, const RibEntry& entry2)
{
  return (entry1.name == entry2.name &&
          entry1.faceId == entry2.faceId &&
          entry1.origin == entry2.origin);
}


Rib::const_iterator
Rib::find(const RibEntry& entry) const
{
  RibTable::const_iterator it = std::find_if(m_rib.begin(), m_rib.end(),
                                             bind(&compareNameFaceProtocol, _1, entry));
  if (it == m_rib.end())
    {
      return end();
    }
  else
    return it;
}


void
Rib::insert(const RibEntry& entry)
{
  RibTable::iterator it = std::find_if(m_rib.begin(), m_rib.end(),
                                       bind(&compareNameFaceProtocol, _1, entry));
  if (it == m_rib.end())
    {
      m_rib.push_front(entry);
    }
  else
    {
      // entry exist, update other fields
      it->flags = entry.flags;
      it->cost = entry.cost;
      it->expires = entry.expires;
    }
}


void
Rib::erase(const RibEntry& entry)
{
  RibTable::iterator it = std::find_if(m_rib.begin(), m_rib.end(),
                                       bind(&compareNameFaceProtocol, _1, entry));
  if (it != m_rib.end())
    {
      m_rib.erase(it);
    }
}

void
Rib::erase(uint64_t faceId)
{
  // Keep it simple for now, with Trie this will be changed.
  RibTable::iterator it = m_rib.begin();
  while (it != m_rib.end())
  {
    if (it->faceId == faceId)
      it = m_rib.erase(it);
    else
      ++it;
  }
}

std::ostream&
operator<<(std::ostream& os, const RibEntry& entry)
{
  os << "RibEntry("
     << "name: " << entry.name
     << " faceid: " << entry.faceId
     << " origin: " << entry.origin
     << " cost: " << entry.cost
     << " flags: " << entry.flags
     << " expires in: " << (entry.expires - time::steady_clock::now())
     << ")";

  return os;
}

} // namespace rib
} // namespace nfd
