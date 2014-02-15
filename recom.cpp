#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>
#include <time.h>
#include <sys/time.h>
#include <jubatus/client/recommender_client.hpp>
#include <jubatus/client/recommender_types.hpp>

using namespace std;
using namespace jubatus;
using namespace jubatus::recommender;
using jubatus::client::common::datum;
using jubatus::recommender::client::recommender;

jubatus::recommender::client::recommender *jubatus_recommender;
map< std::pair<string,string> ,double>::iterator res;


class Result{
public:
   string name;
   double score;
   Result(string name, double score){
      this->name = name;
      this->score = score;
   }
};

bool operator<(const Result& left, const Result& right){
  return left.score < right.score ;
}

bool operator>(const Result& left, const Result& right){
  return left.score > right.score ;
}


void ShowSimilar(string user_id ){
   std::vector<id_with_score> sr = jubatus_recommender->similar_row_from_id( user_id , 4);
   cout <<  user_id <<":"<<endl;
   cout << "   similar to :";
   for (size_t i = 1; i < sr.size(); ++i) {
      cout <<  sr[i].id <<"("<< sr[i].score <<")"<< ", ";
   }
   cout << endl;
}

void ShowRecommend( string user_id, map< std::pair<string,string>, double> *list ){
   vector<Result> result_list;
   Result *result;
   datum complete = jubatus_recommender->complete_row_from_id( user_id );
   cout <<"   recommend "<< user_id <<"("<< complete.num_values.size() <<"): ";
   if( complete.num_values.size() == 1)return;
   for(vector< pair<string, double> >::iterator it = complete.num_values.begin(); it!=complete.num_values.end() ;it++ ){
      //cout << it->first <<"("<< it->second <<")";
      result = new Result( it->first, it->second );
      result_list.push_back( *result );
      delete result;
   }
   sort( result_list.begin(),result_list.end(), std::greater<Result>() );
   int count=0;
   for( vector<Result>::iterator it = result_list.begin(); it!=result_list.end(); it++  ){
      res = list->find( make_pair( user_id, it->name) );
      if( res == list->end() ){
         cout << it->name <<"("<< it->score <<"), ";
         if( ++count >= 5 )break;
      }else{
         //cout <<"already watched!"<<endl;
      }
   }
   cout << endl;
}

void Analyze(string file_name){
}


int main(int argc, char* argv[]) {
   jubatus_recommender = new jubatus::recommender::client::recommender("localhost", 9199, "anime", 5);
   ifstream ifs("data.dat");
   if (!ifs) {
      throw string("cannot open csv file");
   }
   //read file data
   map< std::pair<string,string> ,double>list;
   string user_name, anime_name, rating;
   while ((ifs >> user_name >> anime_name >> rating ) != 0) {
      datum d;
      //cout<<user_name<<endl;
      d.add_number(anime_name, atof(rating.c_str()) );
      jubatus_recommender->update_row(user_name, d);
      list.insert( std::make_pair( std::make_pair( user_name, anime_name), atof(rating.c_str() )) );
      //list.emplace( map_type::key_type( user_name, anime_name), atof(rating.c_str() ) );
   }
   //open data.dat
   Analyze("data.dat");
   vector<string> user_list = jubatus_recommender->get_all_rows();
   for(vector<string>::iterator ut = user_list.begin(); ut!=user_list.end() ;ut++){
      ShowSimilar( *ut );
      ShowRecommend( *ut , &list );
   }
   cout<<endl;
   jubatus_recommender->clear();
   delete jubatus_recommender;
   return 0;
}
