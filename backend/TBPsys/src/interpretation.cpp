#include <chrono>
#include <thread>
#include <../include/interpretation.hpp>
#include <../include/base.hpp>
#include <../include/segment.hpp>


#include <string.h>
#include <vector>

Interpretation::Interpretation(std::shared_ptr<Base> mother, int id):
m_base{mother},
m_video{mother->get_videocap()},
m_img_type{mother->get_img_type()},
m_id{id}
{
  std::cout<<"interpretation\n";
}

int Interpretation::get_id()
{
  return m_id;
}

Interpretation::~Interpretation(){
  std::cout<<"deleting interpretation id:"<<m_id<<"\n";
}

void Interpretation::add_connection(int id, Segment* segment){
  m_mutex_connections.lock();
  m_all_connections[id]=segment;
  m_mutex_connections.unlock();
}

void Interpretation::delete_connection(int id){
  m_mutex_connections.lock();
  m_all_connections.erase(id);
  m_mutex_connections.unlock();

}

void Interpretation::update_connections(){
  m_mutex_connections.lock();
  for (auto it=m_all_connections.begin(); it!=m_all_connections.end(); ++it)
    (*(it->second)).update_interpretation();
  m_mutex_connections.unlock();
}
