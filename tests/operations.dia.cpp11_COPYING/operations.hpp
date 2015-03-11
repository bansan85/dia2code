#ifndef OPERATIONS__HPP
#define OPERATIONS__HPP

/*
License file,
bit-by-bit.
 */

class operations {
    // Operations
    private :
        /**
         * \brief comment
         * \param in (in)
         * \param inOut (in/out) comment2
         * \param undef (???)
         * \param out (out)
         * \return type
         */
        virtual type test (type1 in, type2 inOut, type3 undef = 0, type4 out) = 0;
    public :
        /**
         * \brief comment2
         */
        virtual static test2 ();
        test3 () const;
    protected :
        test4 ();
    public :
        test5 () = delete;
};

#endif
