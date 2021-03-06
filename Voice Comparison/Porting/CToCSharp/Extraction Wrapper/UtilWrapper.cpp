#include <UtilWrapper.h>


namespace ExtractionWrapper{
	DATA_TYPE* UtilWrapper::ConverListToPtr(List<DATA_TYPE> ^ list){
		DATA_TYPE *data = ArrayUtil::CreateZeroArray1D(list->Count);//new DATA_TYPE[list->Count];
		for (int i = 0; i < list->Count; i++){// Count List is Int
			data[i] = list[i];
		}
		return data;
	}

	List<DATA_TYPE>^ UtilWrapper::ConverPtrToList(DATA_TYPE *ptr, COUNT_TYPE size){
		List<DATA_TYPE>^ list = gcnew List<DATA_TYPE>();
		if (ptr != NULL){
			for (COUNT_TYPE i = 0; i < size; i++){
				list->Add(ptr[i]);
			}
		}
		return list;
	}

	List<DATA_TYPE>^ UtilWrapper::ConverVectorToList(std::vector<DATA_TYPE> vec){
		List<DATA_TYPE>^ list = gcnew List<DATA_TYPE>();
		for (COUNT_TYPE i = 0; i < vec.size(); i++){
			list->Add(vec[i]);
		}
		return list;
	}

	List<COUNT_TYPE>^ UtilWrapper::ConverVectorToList(std::vector<COUNT_TYPE> vec){
		List<COUNT_TYPE>^ list = gcnew List<COUNT_TYPE>();
		for (COUNT_TYPE i = 0; i < vec.size(); i++){
			list->Add(vec[i]);
		}
		return list;
	}

	List<List<DATA_TYPE>^>^ UtilWrapper::ConverVectorPtrToListList(std::vector<DATA_TYPE*> vec, COUNT_TYPE col){
		List<List<DATA_TYPE>^>^ list = gcnew List<List<DATA_TYPE>^>();

		for (COUNT_TYPE i = 0; i < vec.size(); i++){
			DATA_TYPE * tmp = vec[i];
			list->Add(ConverPtrToList(tmp, col));
		}
		return list;
	}

	List<DATA_TYPE>^ UtilWrapper::VectorToList(Vector &vec){
		List<DATA_TYPE>^ list = gcnew List<DATA_TYPE>();
		for (COUNT_TYPE j = 0; j < vec.Size(); j++){
			list->Add(vec(j));
		}
		return list;
	}

	Vector UtilWrapper::ListToVector(List<DATA_TYPE>^ list){
		COUNT_TYPE size = list->Count;
		if (size > 0){
			Vector result(size);
			for (COUNT_TYPE c = 0; c < size; c++){
				result(c) = list[c];
			}
			return result;
		}
		return Vector();
	}

	List<List<DATA_TYPE>^>^ UtilWrapper::MatrixToListList(Matrix &vec){
		List<List<DATA_TYPE>^>^ list = gcnew List<List<DATA_TYPE>^>();

		for (COUNT_TYPE i = 0; i < vec.ColumnSize(); i++){
			List<DATA_TYPE> ^col = gcnew List<DATA_TYPE>();
			for (COUNT_TYPE j = 0; j < vec.RowSize(); j++){
				col->Add(vec(j, i));
			}
			list->Add(col);
		}
		return list;
	}

	Matrix UtilWrapper::ListListToMatrix(List<List<DATA_TYPE>^>^ list){
		COUNT_TYPE col = list->Count;
		if (col > 0){
			List<DATA_TYPE>^ vec = list[0];
			COUNT_TYPE row = vec->Count;
			if (row > 0){
				Matrix result(row, col);
				for (COUNT_TYPE c = 0; c < col; c++){
					List<DATA_TYPE> ^colVec = list[c];
					if (colVec->Count == row){
						for (COUNT_TYPE r = 0; r < row; r++){
							result(r, c) = colVec[r];
						}
					}
					else {
						return Matrix();
					}
				}
				return result;
			}
			else {
				return Matrix();
			}
		}

		return Matrix();
	}

	std::vector<DATA_TYPE*> UtilWrapper::ConverListListToVectorPtr(List<List<DATA_TYPE>^>^ vec, COUNT_TYPE &col){
		std::vector<DATA_TYPE*> out;
		for (int i = 0; i < vec->Count; i++){
			List<DATA_TYPE>^ list = vec[i];
			col = list->Count;
			DATA_TYPE* ptr = ConverListToPtr(list);
			out.push_back(ptr);
		}
		return out;
	}

	char *UtilWrapper::ConvertStringToChar(String^ str){
		pin_ptr<const wchar_t> wch = PtrToStringChars(str);
		size_t convertedChars = 0;
		size_t  sizeInBytes = ((str->Length + 1) * 2);
		errno_t err = 0;
		char *tmp = (char *)malloc(sizeInBytes);

		err = wcstombs_s(&convertedChars,
			tmp, sizeInBytes,
			wch, sizeInBytes);
		if (err == 0){
			return tmp;
		}
		return NULL;
	}

	String^ UtilWrapper::ConvertCharToString(char *str){
		if (str != NULL){
			return gcnew String(str);
		}
		else {
			return nullptr;
		}
	}

	std::vector<std::string> UtilWrapper::ConvertListStringToVectorString(List<String ^>^ listStr){
		std::vector<std::string> vFiles;
		for (int i = 0; i < listStr->Count; i++){
			char *file = UtilWrapper::ConvertStringToChar(listStr[i]);
			if (file != NULL){
				vFiles.push_back(std::string(file));
				free(file);
			}
		}
		return vFiles;
	}

	std::vector<Matrix> UtilWrapper::ConvertTripListToVectorMatrix(List<List<List<DATA_TYPE>^>^> ^ data){
		std::vector<Matrix> vData;
		for (int i = 0; i < data->Count; i++){
			Matrix m = UtilWrapper::ListListToMatrix(data[i]);
			vData.push_back(m);
		}
		return vData;
	}

	bool FileWrapper::Write(String ^path, List<DATA_TYPE>^ data){
		COUNT_TYPE size = data->Count;
		DATA_TYPE * dataPtr = UtilWrapper::ConverListToPtr(data);
		char* p = UtilWrapper::ConvertStringToChar(path);
		return File::Write(p, dataPtr, size);
	}

	bool FileWrapper::Write(String ^path, List<List<DATA_TYPE>^>^ data){
		if (data->Count > 0){
			try{
				COUNT_TYPE col = data[0]->Count;
				Array3D dataPtr = UtilWrapper::ConverListListToVectorPtr(data, col);
				char* p = UtilWrapper::ConvertStringToChar(path);
				return File::Write(p, dataPtr, col);
			}
			catch (Exception^ ex){
				// TO DO
				return false;
			}
		}
		return false;
	}

	bool FileWrapper::Read(String ^path, List<List<DATA_TYPE>^>^% data){
		try{
			COUNT_TYPE col = 0;
			Array3D vec;
			char* p = UtilWrapper::ConvertStringToChar(path);
			bool result = File::Read(p, vec, col);
			data = UtilWrapper::ConverVectorPtrToListList(vec, col);
			return result;
		}
		catch (Exception^ ex){
			// TO DO
			return false;
		}
	}

	bool FileWrapper::Read(String ^path, List<DATA_TYPE>^% data){
		COUNT_TYPE col = 0;
		DATA_TYPE* dataPtr = NULL;
		char* p = UtilWrapper::ConvertStringToChar(path);
		bool result = File::Read(p, dataPtr, col);
		data = UtilWrapper::ConverPtrToList(dataPtr, col);
		return result;
	}

	void OptionWrapper::SetLog(int mode){
		OptionUtil::SetEnableLog(mode);
	}

	void OptionWrapper::SeparateLog(){
		OptionUtil::SeparateLog();
	}
};
