#include "file-util.h"

#ifdef OP_WINDOWS
#include <direct.h>
void Util::FileUtil::getFiles(string path, vector<string> &files) {
    //文件句柄
    long   hFile   =   0;
    //文件信息
    struct _finddata_t fileinfo;
    string p;
    if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)
    {
        do
        {
            //如果是目录,迭代之
            //如果不是,加入列表
            if((fileinfo.attrib &  _A_SUBDIR))
            {
                if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)
                    getFiles( p.assign(path).append("\\").append(fileinfo.name), files );
            }
            else
            {
//                files.push_back(p.assign(path).append("\\").append(fileinfo.name) );
                files.push_back(fileinfo.name);
            }
        }while(_findnext(hFile, &fileinfo)  == 0);
        _findclose(hFile);
    }
}
#endif
#ifdef OP_DARWIN

#include <dirent.h>

void Util::FileUtil::getFiles(string path, vector<string> &files) {
    string dirname;
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(path.c_str())) == NULL)
        cout << "Can't open " << dirname << endl;
    while ((dirp = readdir(dp)) != NULL) {
        if(dirp->d_type==8){
            cout << dirp->d_name << endl;
            string name=dirp->d_name;
            files.push_back(name);
        }
    }
    closedir(dp);
}

#endif

