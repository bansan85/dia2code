#ifndef OPERATIONS__HPP
#define OPERATIONS__HPP

class DllExport operations {
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
        DllExport virtual type test (type1 in, type2 inOut, type3 undef = 0, type4 out) = 0;
    public :
        /**
         * \brief comment2
         */
        DllExport virtual static test2 ();
        DllExport test3 () const;
    private :
        DllExport test4 ();
        DllExport test5 ();
};

#endif
