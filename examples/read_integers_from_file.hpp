#pragma once

#include <string>
#include <fstream>

namespace stations
{


/**
 * Interface
 */
class ReadIntegersFromFile
{
 public:
  /**
   * CONSTRUCTOR, DECONSTRUCTOR AND OPTIONS
   */
  ReadIntegersFromFile(std::string const file_name);
  ReadIntegersFromFile & set_chunk_size(std::size_t const chunk_size);

  /**
   * DATA ACCESS
   */
  std::size_t get_chunk_size() const;

  /**
   * MODIFIERS
   */
  std::shared_ptr<std::vector<int> > operator()();

 private:
  std::ifstream file;
  std::size_t chunk_size;
};


/**
 * Implementation
 */
ReadIntegersFromFile::ReadIntegersFromFile(std::string const file_name)
  : file(file_name.c_str())
  , chunk_size(1000)
{}


ReadIntegersFromFile &
ReadIntegersFromFile::set_chunk_size(std::size_t const _chunk_size)
{
  chunk_size = _chunk_size;
  return *this;
}


std::size_t
ReadIntegersFromFile::get_chunk_size() const
{
  return chunk_size;
}


std::shared_ptr<std::vector<int> >
ReadIntegersFromFile::operator()()
{
  std::shared_ptr<std::vector<int> > integers = std::make_shared<std::vector<int> >();
  integers->reserve(chunk_size);

  if (file)
  {
    std::string line;

    while (integers->size() < chunk_size and getline(file, line) and line.size() > 0)
    {
      integers->push_back(stoi(line));
    }
  }

  return integers;
}

} // namespace stations
