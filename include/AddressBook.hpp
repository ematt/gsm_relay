#include <Arduino.h>
#include <EEPROM.h>

class AddressBook {
    public:
    static const size_t NumberMaxLength = 15;
    static const size_t MaxNumbers = 20;
    EERef eepromStart = EERef(0);
    EERef eepromEnd = EERef(eepromStart + MaxNumbers * NumberMaxLength);


    bool addNumber(const String& number)
    {
        return addNumber(findEmptySlot(), number);
    }

    bool addNumber(size_t position, const String& number)
    {
        if(number.length() > NumberMaxLength)
            return false;

        if(position > MaxNumbers)
            return false;

        if(isNumberAllowed(number))
            return false;

        size_t address = position * NumberMaxLength;
        for(size_t i = 0; i < number.length(); i++)
        {
            EEPROM.update(address, number[i]);
            address++;
        }
        EEPROM.update(address, '\0');

        return true;
    }

    bool deleteNumber(size_t position)
    {
        if(position > MaxNumbers)
            return false;

        size_t address = position * NumberMaxLength;
        EEPROM.update(address, 0xFF);

        return true;
    }

    bool positionIsEmpty(size_t position)
    {
        return (EEPROM.read(position * NumberMaxLength) == 0xFF);
    }

    String getNumber(size_t position)
    {
        if(position > MaxNumbers)
            return String("");
        
        uint8_t buffer[NumberMaxLength+1] = {0x00}; 

        for(size_t i = 0; i < NumberMaxLength; i++)
        {
            buffer[i] = EEPROM.read(position * NumberMaxLength + i);
        }

        if(buffer[0] == 0xFF)
        {
            return String("N/A");
        }

        return String((char*)(buffer));
    }

    size_t count()
    {
        size_t count = 0;
        for( size_t i = 0; i < MaxNumbers; i++)
        {
            if(positionIsEmpty(i) == false)
            {
                count++;
            }
        }

        return count;
    }

    size_t findEmptySlot()
    {
        for( size_t i = 0; i < MaxNumbers; i++)
        {
            if(positionIsEmpty(i))
            {
                return i;
            }
        }
        return MaxNumbers;
    }

    bool isNumberAllowed(const String& number)
    {
        for( size_t i = 0; i < MaxNumbers; i++)
        {
            String currentNumber = getNumber(i);
            if(currentNumber.equals(number))
            {
                return true;
            }
        }
        
        return false;
    }

    void erase()
    {
        for(size_t i = 0; i < NumberMaxLength*MaxNumbers; i++)
        {
            EEPROM.write(i, 0xFF);
        }
    }
};
