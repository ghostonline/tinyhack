#include "FileSystem.h"
#include "Path.h"
#include <diag/Log.h>

#include <CoreFoundation/CFURL.h>
#include <CoreFoundation/CFBundle.h>
#include <sys/syslimits.h> /*for PATH_MAX*/

Path filesystem::get_resource_path(const Path& path)
{
    Path resource_path = path;
    auto components = path.split_filename();
    if (!components.filename)
    {
        Log::error("Path does not point to a file: {0}", path);
        return resource_path;
    }
    
    auto filename_ref = CFStringCreateWithBytes(nullptr, reinterpret_cast<const UInt8*>(components.filename.get_ptr()), components.filename.get_size(), kCFStringEncodingUTF8, FALSE);
    auto basepath_ref = CFStringCreateWithBytes(nullptr, reinterpret_cast<const UInt8*>(components.base.get_ptr()), components.base.get_size(), kCFStringEncodingUTF8, FALSE);

    char result[PATH_MAX]; //the buffer containing the resulting path
    CFURLRef fileref = CFBundleCopyResourceURL(
                                               CFBundleGetMainBundle(),
                                               filename_ref,
                                               nullptr,
                                               basepath_ref
                                               );
    if (fileref == 0) {
        Log::error("Unable to construct resource path: {0}", path);
        return resource_path;
    }

    Boolean bresult = CFURLGetFileSystemRepresentation(fileref, true, (UInt8*) result, sizeof(result));

    CFRelease(fileref);
    CFRelease(filename_ref);
    CFRelease(basepath_ref);

    if(bresult)
    {
        resource_path = Path(result);
    }

    return resource_path;
}
