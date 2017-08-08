#include <fc/log/json_console_appender.hpp>
#include <fc/log/log_message.hpp>
#include <fc/thread/unique_lock.hpp>
#include <fc/string.hpp>
#include <fc/variant.hpp>
#include <fc/reflect/variant.hpp>
#ifndef WIN32
#include <unistd.h>
#endif
#include <boost/thread/mutex.hpp>
#define COLOR_CONSOLE 1
#include "console_defines.h"
#include <fc/io/stdio.hpp>
#include <fc/exception/exception.hpp>
#include <iomanip>
#include <sstream>
// 
#include <fc/io/json.hpp>
// 
namespace fc {

   class json_console_appender::impl {
   public:
     config                      cfg;
     color::type                 lc[log_level::off+1];
#ifdef WIN32
     HANDLE                      json_console_handle;
#endif
   };

   json_console_appender::json_console_appender( const variant& args ) 
   :my(new impl)
   {
      configure( args.as<config>() );
   }

   json_console_appender::json_console_appender( const config& cfg )
   :my(new impl)
   {
      configure( cfg );
   }
   json_console_appender::json_console_appender()
   :my(new impl){}


   void json_console_appender::configure( const config& json_console_appender_config )
   { try {
#ifdef WIN32
      my->json_console_handle = INVALID_HANDLE_VALUE;
#endif
      my->cfg = json_console_appender_config;
#ifdef WIN32
         if (my->cfg.stream = stream::std_error)
           my->json_console_handle = GetStdHandle(STD_ERROR_HANDLE);
         else if (my->cfg.stream = stream::std_out)
           my->json_console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

         for( int i = 0; i < log_level::off+1; ++i )
            my->lc[i] = color::console_default;
         for( auto itr = my->cfg.level_colors.begin(); itr != my->cfg.level_colors.end(); ++itr )
            my->lc[itr->level] = itr->color;
   } FC_CAPTURE_AND_RETHROW( (json_console_appender_config) ) }

   json_console_appender::~json_console_appender() {}

   #ifdef WIN32
   static WORD
   #else
   static const char* 
   #endif
   get_console_color(json_console_appender::color::type t ) {
      switch( t ) {
         case json_console_appender::color::red: return CONSOLE_RED;
         case json_console_appender::color::green: return CONSOLE_GREEN;
         case json_console_appender::color::brown: return CONSOLE_BROWN;
         case json_console_appender::color::blue: return CONSOLE_BLUE;
         case json_console_appender::color::magenta: return CONSOLE_MAGENTA;
         case json_console_appender::color::cyan: return CONSOLE_CYAN;
         case json_console_appender::color::white: return CONSOLE_WHITE;
         case json_console_appender::color::console_default:
         default:
            return CONSOLE_DEFAULT;
      }
   }

   boost::mutex& log_mutex() {
    static boost::mutex m; return m;
   }

   void json_console_appender::log( const log_message& m ) {
      
      fc::variant v;
      fc::to_variant(m, v);
      // std::string json_log_message = fc::json::to_pretty_string(v);
      std::string json_log_message = fc::json::to_string(v);

      print(json_log_message, my->lc[m.get_context().get_log_level()] );

      fprintf( out, "\n" );

      if( my->cfg.flush ) fflush( out );
   }

   void json_console_appender::print( const std::string& text, color::type text_color )
   {
      FILE* out = stream::std_error ? stderr : stdout;

      #ifdef WIN32
         if (my->json_console_handle != INVALID_HANDLE_VALUE)
           SetConsoleTextAttribute(my->json_console_handle, get_console_color(text_color));
      #else
         if(isatty(fileno(out))) fprintf( out, "\r%s", get_console_color( text_color ) );
      #endif

      if( text.size() )
         fprintf( out, "%s", text.c_str() ); //fmt_str.c_str() ); 

      #ifdef WIN32
      if (my->json_console_handle != INVALID_HANDLE_VALUE)
        SetConsoleTextAttribute(my->json_console_handle, CONSOLE_DEFAULT);
      #else
      if(isatty(fileno(out))) fprintf( out, "\r%s", CONSOLE_DEFAULT );
      #endif

      if( my->cfg.flush ) fflush( out );
   }

}
