/*
    TextMsgBuffer.h

    Copyright 2014-2019, Will Godfrey

    This file is part of yoshimi, which is free software: you can
    redistribute it and/or modify it under the terms of the GNU General
    Public License as published by the Free Software Foundation, either
    version 2 of the License, or (at your option) any later version.

    yoshimi is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with yoshimi.  If not, see <http://www.gnu.org/licenses/>.

    Modified August 2019
*/

#ifndef TEXTMSGBUFFER_H
#define TEXTMSGBUFFER_H

//#define REPORT_MISCMSG
// for testing message list leaks

#include <list>
#include <string>
#include <semaphore.h>
#include <iostream>

#include "globals.h"


/*
 * This singleton provides a transparent text messaging system.
 * Calling functions only need to recognise integers and strings.
 *
 * Pop is destructive. No two functions should ever have been given
 * the same 'live' ID, but if they do, the second one will get an
 * empty string.
 *
 * Both calls will block, but should be very quick;
 *
 * Normally a message will clear before the next one arrives so the
 * message numbers should remain very low even over multiple instances.
 *
 * Historical note: the miscList used to be a global variable and was
 * accessed through functions mixed in via the MiscFuncs baseclass.
 * Extracted and changed into a Meyer's Singleton by Ichthyostega 8/2019
 */
class TextMsgBuffer
{
        sem_t miscmsglock;
        std::list<std::string> miscList;

        TextMsgBuffer() :
            miscmsglock{},
            miscList{}
        {
            sem_init(&miscmsglock, 0, 1);
        }

        ~TextMsgBuffer()
        {
            sem_destroy(&miscmsglock);
        }

    public:
        /* Meyer's Singleton */
        static TextMsgBuffer& instance()
        {
            static TextMsgBuffer singleton{};
            return singleton;
        }

        void miscMsgInit(void);
        void miscMsgClear(void);
        int miscMsgPush(std::string text);
        std::string miscMsgPop(int pos);
};





inline void TextMsgBuffer::miscMsgInit()
{
    for (int i = 0; i < NO_MSG; ++i)
        miscList.push_back("");
    // we use 255 to denote an invalid entry
}


inline void TextMsgBuffer::miscMsgClear()
{ // catches messge leaks - shirley knot :@)
#ifdef REPORT_MISCMSG
    std::cout << "Msg list cleared" << std::endl;
#endif
    std::list<std::string>::iterator it = miscList.begin();
    for (it = miscList.begin(); it != miscList.end(); ++it)
        *it = "";
}


inline int TextMsgBuffer::miscMsgPush(std::string _text)
{
    if (_text.empty())
        return NO_MSG;
    sem_wait(&miscmsglock);

    std::string text = _text;
    std::list<std::string>::iterator it = miscList.begin();
    int idx = 0;

    while(it != miscList.end())
    {
        if ( *it == "")
        {
            *it = text;
#ifdef REPORT_MISCMSG
            std::cout << "Msg In " << int(idx) << " >" << text << "<" << std::endl;
#endif
            break;
        }
        ++ it;
        ++ idx;
    }
    if (it == miscList.end())
    {
        std::cerr << "miscMsg list full :(" << std::endl;
        idx = -1;
    }

    int result = idx; // in case of a new entry before return
    sem_post(&miscmsglock);
    return result;
}


inline std::string TextMsgBuffer::miscMsgPop(int _pos)
{
    if (_pos >= NO_MSG)
        return "";
    sem_wait(&miscmsglock);

    int pos = _pos;
    std::list<std::string>::iterator it = miscList.begin();
    int idx = 0;

    while(it != miscList.end())
    {
        if (idx == pos)
        {
#ifdef REPORT_MISCMSG
            std::cout << "Msg Out " << int(idx) << " >" << *it << "<" << std::endl;
#endif
            break;
        }
        ++ it;
        ++ idx;
    }
    std::string result = "";
    if (idx == pos)
    {
        swap (result, *it); // in case of a new entry before return
    }
    sem_post(&miscmsglock);
    return result;
}



#endif /*TEXTMSGBUFFER_H*/
