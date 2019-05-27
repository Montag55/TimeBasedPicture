/*
Author:     Phil Jungschlaeger, Lucas Hübner
Date:       27.05.2019
*/
#include <chrono>
#include <thread>
#include <../include/transformation.hpp>
#include <../include/base.hpp>
#include <../include/segment.hpp>


#include <string.h>
#include <vector>

Transformation::Transformation( std::shared_ptr<Base> mother, int id, int offset, int stride):
m_base{mother},
m_id{id},
m_offset{offset},
m_stride{stride},
m_video{mother->get_videocap()},
m_img_type{mother->get_img_type()}
{}

int Transformation::get_upload_specification()
{
  return m_upload_specification;
}

int Transformation::get_id()
{
  return m_id;
}

int Transformation::get_offset()
{
  return m_offset;
}

int Transformation::get_stride()
{
  return m_stride;
}

Transformation::~Transformation(){
  // std::cout << "~Transformation \n";
  // m_base = NULL;
  // m_video = NULL;
  // delete m_all_connections;
}

void Transformation::add_connection( int id, Segment* segment){
  // std::cout<<"add_connection from meta\n";
  m_mutex_connections.lock();
  m_all_connections[id]=segment;
  m_mutex_connections.unlock();
}

void Transformation::delete_connection( int id){
  m_mutex_connections.lock();
  m_all_connections.erase(id);
  m_mutex_connections.unlock();
}

void Transformation::update_connections(){
  m_mutex_connections.lock();
  for (auto it=m_all_connections.begin(); it!=m_all_connections.end(); ++it)
    (*(it->second)).update_transformation();
  m_mutex_connections.unlock();
}

void Transformation::trigger_connections(){
  m_mutex_connections.lock();
  for (auto it=m_all_connections.begin(); it!=m_all_connections.end(); ++it)
    (*(it->second)).trigger_transformation();
  m_mutex_connections.unlock();
}
