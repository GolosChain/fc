#ifndef FC_FWD_HPP_
#define FC_FWD_HPP_
#include <fc/aligned.hpp>

namespace fc {

/**
 *  @brief Used to forward declare value types.
 *
 */
template<typename T,unsigned int S, typename Align=double>
class fwd {
  public:
    template<typename U> fwd( U&& u );
    fwd();

    fwd( const fwd& f );
    fwd( fwd&& f );

    operator const T&()const;
    operator T&();

    T& operator*();
    const T& operator*()const;
    const T* operator->()const;

    T* operator->();
    bool operator !()const;

    template<typename U>
    T& operator = ( U&& u );

    T& operator = ( fwd&& u );
    T& operator = ( const fwd& u );

    ~fwd();
    
  private:
    aligned<S,Align> _store;
};
  

} // namespace fc

#endif