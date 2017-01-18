#include "common.h"

namespace Util {
    class FileUtil {
    public:
      /*************************************************
      Function:       getFiles
      Description:    获取目录下的所有文件列表
      Input:          path  目录地址
      Output:         files 文件名列表
      *************************************************/
      static void getFiles( string path, vector<string>& files);
    };
}
