#include <type_traits>
#include <iterator>
#include <concepts>

namespace pronto {
    namespace raster {
        
        template <typename T>
        concept impls_distance_to = requires (const T it) { it.distance_to(it); };

        // Check for .decrement
        template <typename T>
        concept impls_decrement = requires (T t) { t.decrement(); };
               
        // Check for .equal_to
        template <typename T>
        concept impls_equal_to =
          requires (const T it) {
            { it.equal_to(it) } ->  std::convertible_to<bool>;
          };

        // difference type default
        template <typename>
        struct infer_difference_type { using type = std::ptrdiff_t; };

        //difference type from 'distance_to'
        template <impls_distance_to T>
        struct infer_difference_type<T> {
            static const T& it;
            using type = decltype(it.distance_to(it));
        };
        // resolve difference type
        template <typename T>
        using infer_difference_type_t = typename infer_difference_type<T>::type;

        // Check for .advance
        template <typename T>
        concept impls_advance =
            requires (T it, const infer_difference_type_t<T> offset) {
            it.advance(offset);
        };

        // We can meet "random access" if it provides
         // both .advance() and .distance_to()
        template <typename T>
        concept meets_random_access =
            impls_advance<T> &&
            impls_distance_to<T>;

        // We meet `bidirectional` if we are random_access, OR we have .decrement()
        template <typename T>
        concept meets_bidirectional =
            meets_random_access<T>
            || impls_decrement<T>;

        // Detect if the iterator declares itself to be a single-pass iterator.
        template <typename T>
        concept decls_single_pass = bool(T::single_pass_iterator);


        // value_type for true reference
        template <typename T>
        struct infer_value_type {
            static const T& _it;
            using type = std::remove_cvref_t<decltype(*_it)>;
        };

        //value type for proxy reference, must have value_type
        template <typename T>
            requires requires { T::value_type; }
        struct infer_value_type<T> {
            using type = typename T::value_type;
        };


        // resolve value type
        template <typename T>
        using infer_value_type_t = infer_value_type<T>::type;


        template <typename Arg, typename Iter>
        concept difference_type_arg =
          std::convertible_to<Arg, infer_difference_type_t<Iter>>;

        //NOT IMPLEMENTING SENTINELS UNTIL ACTUALLY NEEDED
        //template <typename Iter, typename T>
        //concept iter_sentinel_arg = std::same_as<T, typename T::sentinel_type>;
	
        template <typename Derived>
        class iterator_facade {

        public:
          using self_type = Derived;
	
          decltype(auto) operator*() const {
            return _self().dereference();
          }
    
          self_type& operator++() {
            _self().increment();
            return _self();
          }
  
          auto operator++(int) {
              if constexpr (decls_single_pass<self_type>) {
                  ++* this;
              }
              else {
                  auto copy = _self();
                  ++* this;
                  return copy;
              }
          }
  
          self_type& operator--()
            requires impls_decrement<self_type>
          {
            _self().decrement();
            return _self();
          }
  
          self_type operator--(int)
            requires impls_decrement<self_type>
          {
            auto copy = *this;
            --*this;
            return copy;
          }
      
          friend self_type& 
          operator+=(self_type& self, difference_type_arg<self_type> auto offset)
            requires impls_advance<self_type>
          {
            self.advance(offset);
            return self;
          }

          friend self_type
          operator+(self_type left, difference_type_arg<self_type> auto offset) 
            requires impls_advance<self_type>
          {
            return left += offset;
          }

          friend self_type
          operator+(difference_type_arg<self_type> auto offset, self_type right) 
            requires impls_advance<self_type>
          {
            return right += offset;
          }

          friend self_type
          operator-(self_type left, difference_type_arg<self_type> auto offset) 
            requires impls_advance<self_type>
          {
            return left + -offset;
          }

          friend self_type&
          operator-=(self_type& left, difference_type_arg<self_type> auto offset)
            requires impls_advance<self_type>
          {
            return left = left - offset;
          }

          decltype(auto) operator[](difference_type_arg<self_type> auto offset) const
            requires impls_advance<self_type>
          {
            return *(_self() + offset);
          }
  
          friend auto
          operator-(const self_type& left, const self_type& right)
            requires impls_distance_to<self_type>
          {
            return right.distance_to(left);
          }
  
          friend bool 
          operator==(const self_type& left,const self_type& right) {
            return left.equal_to(right);
          }  
  
          friend auto
          operator<=>(const self_type& left, const self_type& right)
            requires impls_distance_to<self_type>
          {
            return (left - right) <=> 0;
          }

          // NOT IMPLEMENTING SENTINELS UNTIL ACTUALLY NEEDED
          //friend bool 
          //operator==(const self_type& self, iter_sentinel_arg<self_type> auto) {
          //  return self.at_end();
          // }

        private:
          self_type& _self() {
            return static_cast<self_type&>(*this);
          }
  
          const self_type& _self() const {
            return static_cast<const self_type&>(*this);
          }
        };
        using std::iterator_traits;

        template <typename Iter>
          requires std::is_base_of_v<iterator_facade<Iter>, Iter>
        struct iterator_traits<Iter> {

        private: 
          static const Iter& _it;

        public:
          using reference = decltype(*_it);
          using difference_type = pronto::raster::infer_difference_type_t<Iter>;
          using value_type = pronto::raster::infer_value_type_t<Iter>;

          class io_iterator_tag : public input_iterator_tag, public output_iterator_tag
          {};

          // TODO - This needs to be updated to consider output_iterator, input_and_output_iterator
          using iterator_category =
            std::conditional_t<
              pronto::raster::meets_random_access<Iter>,
                std::random_access_iterator_tag,
                std::conditional_t< 
                  pronto::raster::meets_bidirectional<Iter>,
                  std::bidirectional_iterator_tag,
                  std::conditional_t<
                    Iter::is_single_pass,
                    std::conditional_t<
                       Iter::is_mutable,
                       std::input_iterator_tag,
                       io_iterator_tag>,
                    std::forward_iterator_tag
                >
              >
            >;

          using iterator_concept = iterator_category;
        };

    }
}
