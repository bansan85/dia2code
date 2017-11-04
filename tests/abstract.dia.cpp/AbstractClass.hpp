#ifndef ABSTRACTCLASS__HPP
#define ABSTRACTCLASS__HPP

class AbstractClass {
    // Operations
    public :
        virtual void AbstractOperation () = 0;
        virtual void VirtualOperation ();
        void NonAbstractOperation ();
};

#endif
