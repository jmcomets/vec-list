#ifndef LLIST_H
#define LLIST_H

#include <cassert>
#include <iterator>
#include <optional>
#include <unordered_set>
#include <vector>

template <typename T>
class List
{
    public:
        typedef T value_type;

        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        typedef value_type& reference;
        typedef const value_type& const_reference;

        typedef value_type* pointer;
        typedef const value_type* const_pointer;

    private:
        struct Node
        {
            std::optional<T> value;
            std::optional<size_type> next_id;
            std::optional<size_type> prev_id;
        };

        struct Entry
        {
            size_type id;
            Node& node;
        };

        template <typename V>
        class It: public std::iterator<
                  std::bidirectional_iterator_tag,
                  V, difference_type, V*, V&>
        {
            typedef It<V> Self;

            // TODO handle constness here
            typedef List<T> Ls;

            friend class List;

            public:
                explicit It(Ls& ls, std::optional<size_type> id = std::nullopt):
                    m_ls(&ls),
                    m_id(id)
                {
                }

                template <typename U>
                It(const It<U>& other):
                    m_ls(other.m_ls),
                    m_id(other.m_id)
                {
                }

                Self& operator++()
                {
                    if (m_id)
                    {
                        const Node& node = m_ls->m_nodes[*m_id];
                        m_id = node.next_id;
                    }
                    return *this;
                }

                Self operator++(int)
                {
                    Self retval = *this;
                    ++(*this);
                    return retval;
                }

                Self& operator--()
                {
                    if (m_id)
                    {
                        const Node& node = m_ls->m_nodes[*m_id];
                        m_id = node.prev_id;
                    }
                    else
                    {
                        // we're at the end, use the last id
                        m_id = m_ls->m_last_id;
                    }
                    return *this;
                }

                Self operator--(int)
                {
                    Self retval = *this;
                    --(*this);
                    return retval;
                }

                bool operator==(const Self& other) const
                {
                    return m_ls == other.m_ls && m_id == other.m_id;
                }

                bool operator!=(const Self& other) const
                {
                    return !(*this == other);
                }

                V& operator*() const
                {
                    auto& node = m_ls->m_nodes[*m_id];
                    return *node.value;
                }

                V* operator->() const
                {
                    auto& node = m_ls->m_nodes[*m_id];
                    return &node.value;
                }

            private:
                Ls* m_ls;
                std::optional<size_type> m_id;
        };

    public:
        explicit List():
            m_max_id(0)
        {
        }

        List(size_type count, const T& value);
        explicit List(size_type count);

        template <class InputIt>
        List(InputIt first, InputIt last);

        List(const List& other) = default;
        List(List&& other) = default;

        List& operator=(const List& other) = default;

        List(std::initializer_list<T> init);

        typedef It<value_type> iterator;
        typedef It<const value_type> const_iterator;

        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        // assign - assigns values to the container

        // Appends the given element value to the end of the container.
        //      1) The new element is initialized as a copy of value.
        //      2) value is moved into the new element.
        //
        // No iterators or references are invalidated
        void push_back(T&& value)
        {
            insert(end(), std::move(value));
        }

        // Prepends the given element value to the beginning of the container.
        //
        // No iterators or references are invalidated.
        void push_front(T&& value)
        {
            insert(begin(), std::move(value));
        }

        // Removes the first element of the container. If there are no elements
        // in the container, the behavior is undefined.
        //
        // References and iterators to the erased element are invalidated.
        void pop_front()
        {
            erase(begin());
        }

        // Removes the last element of the container.
        //
        // Calling pop_back on an empty container is undefined.
        //
        // References and iterators to the erased element are invalidated.
        void pop_back()
        {
            erase(end());
        }

        // Returns the number of elements in the container, i.e. std::distance(begin(), end()).
        size_type size() const noexcept
        {
            return nb_ids();
        }

        // Checks if the container has no elements, i.e. whether begin() == end().
        bool empty() const noexcept
        {
            return size() == 0;
        }

        // Returns a reference to the first element in the container.
        //
        // Calling front on an empty container is undefined.
        reference front()
        {
            return *begin();
        }

        // Returns a reference to the first element in the container (const version).
        //
        // Calling front on an empty container is undefined.
        const_reference front() const
        {
            return *cbegin();
        }

        // Returns reference to the last element in the container.
        //
        // Calling back on an empty container is undefined.
        reference back()
        {
            auto tmp = end();
            --tmp;
            return *tmp;
        }

        // Returns reference to the last element in the container (const version).
        //
        // Calling back on an empty container is undefined.
        const_reference back() const
        {
            auto tmp = cend();
            --tmp;
            return *tmp;
        }

        iterator begin()
        {
            return iterator(*this, m_first_id);
        }

        const_iterator begin() const
        {
            return cbegin();
        }

        const_iterator cbegin() const
        {
            return const_iterator(*this, m_first_id);
        }

        iterator end()
        {
            return iterator(*this);
        }

        const_iterator end() const
        {
            return cend();
        }

        const_iterator cend() const
        {
            return const_iterator(*this);
        }

        reverse_iterator rbegin()
        {
            return reverse_iterator(begin());
        }

        const_reverse_iterator rbegin() const
        {
            return rcbegin();
        }

        const_reverse_iterator rcbegin() const
        {
            return reverse_iterator(cbegin());
        }

        reverse_iterator rend()
        {
            return reverse_iterator(end());
        }

        const_reverse_iterator rend() const
        {
            return rcend();
        }

        const_reverse_iterator rcend() const
        {
            return reverse_iterator(cend());
        }

        void clear()
        {
            m_nodes.clear();
            m_free_ids.clear();
            m_first_id = m_last_id = std::nullopt;
            m_max_id = 0;
        }

        // Inserts value before the location pointed to by pos
        //
        // No iterators or references are invalidated.
        //
        // Returns an iterator pointing to the inserted value
        iterator insert(const_iterator pos, T&& value)
        {
            Entry e = put(std::move(value));

            // no id => end
            if (!pos.m_id)
            {
                if (!m_last_id)
                {
                    assert(!m_first_id);
                    m_first_id = m_last_id = e.id;
                }
                else
                {
                    Node& last_node = m_nodes[*m_last_id];
                    last_node.next_id = e.id;
                    e.node.prev_id = m_last_id;
                    m_last_id = e.id;
                }

                return iterator(*this, m_last_id);
            }

            size_type next_id = *pos.m_id;
            auto& next_node = m_nodes[next_id];

            // no prev id => begin
            if (!next_node.prev_id)
            {
                if (!m_first_id)
                {
                    assert(!m_last_id);
                    m_first_id = m_last_id = e.id;
                }
                else
                {
                    Node& first_node = m_nodes[*m_first_id];
                    first_node.prev_id = e.id;
                    e.node.next_id = m_first_id;
                    m_first_id = e.id;
                }

                return iterator(*this, m_first_id);
            }

            size_type prev_id = *next_node.prev_id;
            auto& prev_node = m_nodes[prev_id];

            prev_node.next_id = e.id;
            next_node.prev_id = e.id;
            e.node.prev_id = prev_id;
            e.node.next_id = next_id;
            return iterator(*this, e.id);
        }

        // Inserts count copies of the value before the location pointed to by pos.
        //
        // No iterators or references are invalidated.
        //
        // Returns an iterator pointing to the first element inserted, or pos if count==0.
        iterator insert(const_iterator pos, size_type count, const T& value)
        {
            iterator it = pos;
            for (; count > 0; --count)
            {
                it = insert(pos, value);
            }
            return it;
        }

        // Inserts elements from range [first, last) before the location pointed to by pos.
        //
        // The behavior is undefined if first and last are iterators into *this.
        //
        // No iterators or references are invalidated.
        //
        // Returns an iterator pointing to the first element inserted, or pos if first==last.
        template <class InputIt>
        iterator insert(const_iterator pos, InputIt first, InputIt last)
        {
            iterator it = pos;
            for (; first != last; ++first)
            {
                it = insert(pos, *first);
            }
            return it;
        }

        // Inserts elements from initializer list ilist.
        //
        // No iterators or references are invalidated.
        //
        // Returns an iterator pointing to the first element inserted, or pos
        // if ilist is empty.
        iterator insert(const_iterator pos, std::initializer_list<T> ilist)
        {
            return insert(pos, ilist.begin(), ilist.end());
        }

        // Removes the element at pos.
        //
        // References and iterators to the erased elements are invalidated. Other references
        // and iterators are not affected.
        //
        // The iterator pos must be valid and dereferenceable. Thus the end() iterator (which
        // is valid, but is not dereferencable) cannot be used as a value for pos.
        //
        // Returns the iterator following the last removed element.
        iterator erase(const_iterator pos)
        {
        }

        // Removes the elements in the range [first; last).
        //
        // References and iterators to the erased elements are invalidated. Other references
        // and iterators are not affected.
        //
        // The iterator pos must be valid and dereferenceable. Thus the end() iterator (which
        // is valid, but is not dereferencable) cannot be used as a value for pos.
        //
        // Returns the iterator following the last removed element.
        iterator erase(const_iterator first, const_iterator last)
        {
        }

        // Modifiers
        // emplace - constructs element in-place
        // emplace_back - constructs an element in-place at the end
        // emplace_front - constructs an element in-place at the beginning
        // resize - changes the number of elements stored
        // swap - swaps the contents
        // Operations
        // merge - merges two sorted lists
        // splice - moves elements from another list
        // remove remove_if - removes elements satisfying specific criteria
        // reverse - reverses the order of the elements
        // unique - removes consecutive duplicate elements
        // sort - sorts the elements

    private:
        Entry put(T&& value)
        {
            size_type id;
            if (!m_free_ids.empty())
            {
                auto it = m_free_ids.begin();
                id = *it;
                m_free_ids.erase(it);
            }
            else
            {
                id = m_max_id++;
            }

            // resize the vec if needed
            if (id <= m_nodes.size())
            {
                m_nodes.resize(id + 1);
            }

            Node& node = m_nodes[id];
            node.value = value;
            node.next_id = node.prev_id = std::nullopt;
            return { id, node };
        }

        void pop(size_type id)
        {
            m_free_ids.insert(id);
        }

        size_type nb_ids() const noexcept
        {
            return m_max_id - m_free_ids.size();
        }

    private:
        std::vector<Node> m_nodes;

        std::optional<size_type> m_first_id;
        std::optional<size_type> m_last_id;

        std::unordered_set<size_type> m_free_ids;
        size_type m_max_id;
};

template <typename T>
bool operator==(const List<T>& lhs, const List<T>& rhs);

template <typename T>
bool operator!=(const List<T>& lhs, const List<T>& rhs);

template <typename T>
bool operator<(const List<T>& lhs, const List<T>& rhs);

template <typename T>
bool operator<=(const List<T>& lhs, const List<T>& rhs);

template <typename T>
bool operator>(const List<T>& lhs, const List<T>& rhs);

template <typename T>
bool operator>=(const List<T>& lhs, const List<T>& rhs);

template <typename T>
void swap(List<T>& lhs, List<T>& rhs);

#endif // LLIST_H
