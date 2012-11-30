#ifndef IO_HPP
#define IO_HPP

class io {
public:
    virtual void write(char) = 0;
    virtual char read() = 0;
};

#endif
