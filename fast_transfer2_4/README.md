# FastTransfer
## UA Robotics Team's custom protocol for serial communication.

### Adding FastTransfer to a MPLAB Project

Here is a simple procedure for adding the FastTransfer library to
an existing project in Microchip's MPLAB IDE:

1. Clone the repository:

```
git clone https://github.com/s-collins/fast_transfer.git
```

2. Open the cloned repository in your file explorer and navigate to the
   "src/fast_transfer" subdirectory.  Copy this directory and save it
   somewhere you can find it easily later.

   > **Note:** This repo uses the CMake build system generator to compile and
   > link a suite of unit tests, but you don't want to include
   > all of the build scripts and test code in your project.  To make it
   > easy to transplant FastTransfer into an MPLAB project, all of the
   > library's source code is isolated inside of the subdirectory mentioned
   > above.
   
3. Open an existing project in MPLAB IDE.  Right-click on the "Source Files"
   directory and select "Add Existing Items from Folders". This will open
   a dialog that you can use to navigate to the location on your system
   where you copied the "fast_transfer" directory.
   
4. **Important:** After you locate the "fast_transfer" directory, make
   sure that you select "All Files" from the drop-down menu labeled
   "Files of type:".  This ensures that both the header files and the
   source files will be added to your project.
   
5. Select the "fast_transfer" directory and click the "Add" button.

6. Now, you can include the header "ft.h" in your code, as in the example below:

```C
// Example: Incomplete program for demonstrating FastTransfer API

#include "ft.h"

//--------------------------------------------------------------------
// Callback Functions (user must implement these)
//--------------------------------------------------------------------

void put (uint8_t byte)
{
    /* Send a byte to a UART transmit buffer. */
}

uint8_t get ()
{
    /* Return a byte from the front of a UART receive buffer */
}

bool empty ()
{
    /* Return true iff a UART receive buffer has no bytes */
}

//--------------------------------------------------------------------
// Main Function
//--------------------------------------------------------------------

int main ()
{
    // create a handle to a FastTransfer instance
    FT_t * ft_handle = FT_Create(MY_ADDRESS, put, get, empty);
    
    while (1)
    {
        // parse incoming messages and write the resulting
        // data values into the array associated with the
        // handle
        FT_Receive(ft_handle);
        
        // if value at some index has changed, read it
        if (FT_Modified(ft_handle, DRIVE_INDEX))
        {
            drive_signal = FT_Read(ft_handle, DRIVE_INDEX);
        }
        
        // populate the payload of a packet
        FT_ToSend(ft_handle, CURRENT_INDEX, current);
        FT_ToSend(ft_handle, VOLTAGE_INDEX, voltage);
        
        // send the packet
        FT_Send(ft_handle, CONTROLLER_ADDRESS);
        
        /* ... */
    }
    
    return 0;
}
```
