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
        virtual static void test2 ();
        void test3 () const;
    protected :
        void test4 ();
    private :
        void test5 ();
};

#endif
