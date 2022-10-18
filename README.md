# WMF metadata manipulator

With this tool you can list and remove WMF record structures to play around with this incomprehensible graphics format.

The code is pretty ugly because this tool is rushed.

### Build:
`g++ wmfextract.cxx`

### Use:
`./a.out MyFile.wmf`

Should print some interesting debugging info before the menu

`l` to list structures (view which are scheduled for deletion too)    
`d` to schedule a structure for deletion upon save    
`r` to unschedule a structure from deletion    
`s` to save as (doesn't modify original file)    
`q` to quit    