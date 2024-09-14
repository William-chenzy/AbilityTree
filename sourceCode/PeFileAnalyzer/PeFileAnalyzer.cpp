#include "ui_PeFileAnalyzer.h"
#include "PeFileAnalyzer.h"
#include <QDesktopWidget>
#include <QTextCodec>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QMessageBox>
#include <iostream>
#include <fstream>
using namespace std;

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

PeFileAnalyzer::PeFileAnalyzer(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::PeFileAnalyzer)
{
	ui->setupUi(this);
	QRect deskRect = QApplication::desktop()->availableGeometry();
	if (deskRect.width() < width() || deskRect.height() < height()) {
		this->setBaseSize(deskRect.width(), deskRect.height());
		this->move(0, 0);
	}
	else this->move((deskRect.width() - width()) / 2, (deskRect.height() - height()) / 2);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));

	ui->treeWidget->setHeaderHidden(true);
	ui->tableWidget->verticalHeader()->hide();
	ui->tableWidget->setColumnWidth(0, 100);
	ui->tableWidget->setColumnWidth(1, 10);
	ui->tableWidget->setColumnWidth(2, 10);
	ui->tableWidget->setColumnWidth(3, 10);
	ui->tableWidget->setColumnWidth(4, 10);
	ui->tableWidget->setColumnWidth(5, 10);
	ui->tableWidget->setColumnWidth(6, 10);
	ui->tableWidget->setColumnWidth(7, 10);
	ui->tableWidget->setColumnWidth(8, 10);
	ui->tableWidget->setColumnWidth(9, 10);
	ui->tableWidget->setColumnWidth(10, 10);
	ui->tableWidget->setColumnWidth(11, 10);
	ui->tableWidget->setColumnWidth(12, 10);
	ui->tableWidget->setColumnWidth(13, 10);
	ui->tableWidget->setColumnWidth(14, 10);
	ui->tableWidget->setColumnWidth(15, 10);
	ui->tableWidget->setColumnWidth(16, 10);
	ui->tableWidget->setColumnWidth(17, 210);
	this->setAcceptDrops(true);
	ui->centralwidget->setAcceptDrops(true);
	ui->tableWidget->setAcceptDrops(true);
	ui->treeWidget->setAcceptDrops(true);
}

bool PeFileAnalyzer::ReadFile() {
	fstream file(file_name.toLocal8Bit().toStdString(), ios::in | ios::binary);
	if (!file.is_open())MSG_RET("文件打开失败!");
	while (!file.eof())pehead.raw_val.push_back(file.get());
	file.close();

	const char* v_ptr = pehead.raw_val.data();
	pehead.dos = (_IMAGE_DOS_HEADER*)v_ptr;
	if (pehead.dos->e_magic != 0x5a4d)MSG_RET("非标准PE文件!");
	pehead.dos_info.append(v_ptr + 64, pehead.dos->e_lfanew - 64);
	v_ptr += pehead.dos->e_lfanew;
	pehead.nt = (_IMAGE_NT_HEADERS*)v_ptr;
	pehead.nt64 = (_IMAGE_NT_HEADERS64*)v_ptr;
	if (pehead.nt->Signature != 0x4550)MSG_RET("非标准PE文件!");
	pehead.is_x32 = (pehead.nt->OptionalHeader.Magic == 0x10b);

	v_ptr += (pehead.is_x32 ? sizeof(_IMAGE_NT_HEADERS) : sizeof(_IMAGE_NT_HEADERS64));
	pehead.section = (_IMAGE_SECTION_HEADER*)v_ptr;

	auto vad = (pehead.is_x32 ? pehead.nt->OptionalHeader.DataDirectory[1].VirtualAddress : pehead.nt64->OptionalHeader.DataDirectory[1].VirtualAddress);
	auto foa_ = RvaToFoa(vad + pehead.nt->OptionalHeader.ImageBase);
	if (foa_) pehead.imp = (_IMAGE_IMPORT_DESCRIPTOR*)(pehead.raw_val.data() + foa_);

	return true;
}

void PeFileAnalyzer::ShowValue() {
	while (ui->tableWidget->rowCount())ui->tableWidget->removeRow(0);
	ui->treeWidget->clear();

	unsigned long long stpt = 0;
	auto add_item = [&](QString msg, int len, QTreeWidgetItem* father = nullptr)->QTreeWidgetItem* {
		QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setText(0, msg);
		item->setWhatsThis(0, QString("%1,%2").arg(stpt).arg(len));
		if (!father)ui->treeWidget->addTopLevelItem(item);
		else father->addChild(item);
		stpt += len;
		return item;
	};

	auto dos_item = add_item("DOS Header", 0);
	if(dos_item)
	{
		add_item(w_to_str("Magic number:   ", pehead.dos->e_magic).c_str(), 2, dos_item);
		add_item(w_to_int("Bytes on last page of file:   ", pehead.dos->e_cblp).c_str(), 2, dos_item);
		add_item(w_to_int("Pages in file:   ", pehead.dos->e_cp).c_str(), 2, dos_item);
		add_item(w_to_int("Relocations:   ", pehead.dos->e_crlc).c_str(), 2, dos_item);
		add_item(w_to_int("Size of header in paragraphs:   ", pehead.dos->e_cparhdr).c_str(), 2, dos_item);
		add_item(w_to_int("Minimum extra paragraphs needed:   ", pehead.dos->e_minalloc).c_str(),  2, dos_item);
		add_item(w_to_int("Maximum extra paragraphs needed:   ", pehead.dos->e_maxalloc).c_str(), 2, dos_item);
		add_item(w_to_hex("Initial (relative) SS value:   0x", &pehead.dos->e_ss).c_str(), 2, dos_item);
		add_item(w_to_hex("Initial SP value:   0x", &pehead.dos->e_sp).c_str(), 2, dos_item);
		add_item(w_to_int("Checksum:   ", pehead.dos->e_csum).c_str(), 2, dos_item);
		add_item(w_to_hex("Initial IP value:  0x", &pehead.dos->e_ip).c_str(), 2, dos_item);
		add_item(w_to_hex("Initial (relative) CS value:    0x", &pehead.dos->e_cs).c_str(), 2, dos_item);
		add_item(w_to_int("File address of relocation table:   ", pehead.dos->e_lfarlc).c_str(), 2, dos_item);
		add_item(w_to_int("Overlay number:   ", pehead.dos->e_ovno).c_str(), 2, dos_item);
		add_item(w_to_hex("Reserved words:   0x", pehead.dos->e_res, 4).c_str(), 8, dos_item);
		add_item(w_to_hex("OEM identifier (for e_oeminfo):   0x", &pehead.dos->e_oemid).c_str(), 2, dos_item);
		add_item(w_to_hex("OEM information:   0x", &pehead.dos->e_oeminfo).c_str(), 2, dos_item);
		add_item(w_to_hex("Reserved words:   0x", pehead.dos->e_res2, 10).c_str(), 20, dos_item);
		add_item(dw_to_hex("PE头偏移地址:   0x", (DWORD*)&pehead.dos->e_lfanew).c_str(), 4, dos_item);
		add_item(s_to_fos("Dos stub:   ", pehead.dos_info).c_str(), (int)pehead.dos_info.length(), dos_item);
	}

	auto nt_item = add_item("NT Header", 0);
	add_item(dw_to_str("Signature:   ", pehead.nt->Signature).c_str(), 4, nt_item);

	auto pe_item = add_item("PE Header", 0, nt_item);
	if (pe_item){
		if(pehead.nt->FileHeader.Machine == 0x14c) add_item("程序支持的CPU:   x86", 2, pe_item);
		else if (pehead.nt->FileHeader.Machine == 0x200) add_item("程序支持的CPU:   Inter IPF", 2, pe_item);
		else if (pehead.nt->FileHeader.Machine == 0x8664) add_item("程序支持的CPU:   x64", 2, pe_item);
		else add_item("程序支持的CPU:   Unknow", 2, pe_item);
		add_item(w_to_int("节的数量:   ", pehead.nt->FileHeader.NumberOfSections).c_str(),2, pe_item);
		add_item(dw_to_int("编译器填写的时间戳:   ", pehead.nt->FileHeader.TimeDateStamp).c_str(), 4, pe_item);
		add_item(dw_to_hex("指向符号表:   0x", &pehead.nt->FileHeader.PointerToSymbolTable).c_str(), 4, pe_item);
		add_item(dw_to_int("符号表中的符号个数:   ", pehead.nt->FileHeader.NumberOfSymbols).c_str(), 4, pe_item);
		add_item(w_to_hex("可选PE头结构大小:   0x", &pehead.nt->FileHeader.SizeOfOptionalHeader).c_str(), 2, pe_item);
		add_item(w_to_hex("文件属性:   0x", &pehead.nt->FileHeader.Characteristics).c_str(), 2, pe_item);
	}

	auto op_item = add_item("OPTIONAL Header", 0, nt_item);
	if (op_item) {
		if (pehead.nt->OptionalHeader.Magic == 0x10b) add_item("该文件是一个可执行的映像(32位) ", 2, op_item);
		else if (pehead.nt->OptionalHeader.Magic == 0x20b) add_item("该文件是一个可执行的映像(64位) ", 2, op_item);
		else if (pehead.nt->OptionalHeader.Magic == 0x107) add_item("该文件是ROM镜像 ", 2, op_item);
		else  add_item("未知的文件属性 ", 2, op_item);
		add_item(w_to_int("链接器的主要版本号:   ", pehead.nt->OptionalHeader.MajorLinkerVersion).c_str(), 1, op_item);
		add_item(w_to_int("链接器的次要版本号:   ", pehead.nt->OptionalHeader.MinorLinkerVersion).c_str(), 1, op_item);
		add_item(dw_to_int("代码段的大小:   ", pehead.nt->OptionalHeader.SizeOfCode).c_str(), 4, op_item);
		add_item(dw_to_int("初始化数据段的大小:   ", pehead.nt->OptionalHeader.SizeOfInitializedData).c_str(), 4, op_item);
		add_item(dw_to_int("未初始化数据段的大小:   ", pehead.nt->OptionalHeader.SizeOfUninitializedData).c_str(), 4, op_item);
		add_item(dw_to_hex("程序入口(AddressOfEntryPoint):   0x", &pehead.nt->OptionalHeader.AddressOfEntryPoint).c_str(), 4, op_item);
		add_item(dw_to_hex("代码开始的基址(BaseOfCode):   0x", &pehead.nt->OptionalHeader.BaseOfCode).c_str(), 4, op_item);
		if (!pehead.is_x32) add_item(ldw_to_hex("内存镜像基址(ImageBase):   0x", &pehead.nt64->OptionalHeader.ImageBase).c_str(), 8, op_item);
		else {
			add_item(dw_to_hex("数据开始的基址(BaseOfData):   0x", &pehead.nt->OptionalHeader.BaseOfData).c_str(), 4, op_item);
			add_item(dw_to_hex("内存镜像基址(ImageBase):   0x", &pehead.nt->OptionalHeader.ImageBase).c_str(), 4, op_item);
		}
		add_item(dw_to_hex("内存对齐:   0x", &pehead.nt->OptionalHeader.SectionAlignment).c_str(), 4, op_item);
		add_item(dw_to_hex("文件对齐:   0x", &pehead.nt->OptionalHeader.FileAlignment).c_str(), 4, op_item); 
		add_item(w_to_int("标识操作系统主版本号:   ", pehead.nt->OptionalHeader.MajorOperatingSystemVersion).c_str(), 2, op_item);
		add_item(w_to_int("标识操作系统次版本号:   ", pehead.nt->OptionalHeader.MinorOperatingSystemVersion).c_str(), 2, op_item);
		add_item(w_to_int("PE文件自身的主版本号:   ", pehead.nt->OptionalHeader.MajorImageVersion).c_str(), 2, op_item);
		add_item(w_to_int("PE文件自身的次版本号:   ", pehead.nt->OptionalHeader.MinorImageVersion).c_str(), 2, op_item);
		add_item(w_to_int("运行所需子系统主版本号:   ", pehead.nt->OptionalHeader.MajorSubsystemVersion).c_str(), 2, op_item);
		add_item(w_to_int("运行所需子系统次版本号:   ", pehead.nt->OptionalHeader.MinorSubsystemVersion).c_str(), 2, op_item);
		add_item(dw_to_int("子系统版本的值，必须为0:   ", pehead.nt->OptionalHeader.Win32VersionValue).c_str(), 4, op_item);
		add_item(dw_to_int("Image大小:   ", pehead.nt->OptionalHeader.SizeOfImage).c_str(), 4, op_item);
		add_item(dw_to_int("所有头+节表按照文件对齐后的大小:   ", pehead.nt->OptionalHeader.SizeOfHeaders).c_str(), 4, op_item);
		add_item(dw_to_int("校验和:   ", pehead.nt->OptionalHeader.CheckSum).c_str(), 4, op_item);
		if(pehead.nt->OptionalHeader.Subsystem == 0) add_item("未知的子系统", 2, op_item);
		else if (pehead.nt->OptionalHeader.Subsystem == 1) add_item("不需要子系统(设备驱动程序和本机系统进程)", 2, op_item);
		else if (pehead.nt->OptionalHeader.Subsystem == 2) add_item("Windows图形用户界面子系统", 2, op_item);
		else if (pehead.nt->OptionalHeader.Subsystem == 3) add_item("Windows字符模式用户界面(CUI)子系统", 2, op_item);
		else if (pehead.nt->OptionalHeader.Subsystem == 5) add_item("OS/2 CUI子系统", 2, op_item);
		else if (pehead.nt->OptionalHeader.Subsystem == 7) add_item("POSIX CUI子系统", 2, op_item);
		else if (pehead.nt->OptionalHeader.Subsystem == 9) add_item("Windows CE系统", 2, op_item);
		else if (pehead.nt->OptionalHeader.Subsystem == 10) add_item("可扩展固件接口(EFI)应用程序", 2, op_item);
		else if (pehead.nt->OptionalHeader.Subsystem == 11) add_item("带有引导服务的EFI驱动程序", 2, op_item);
		else if (pehead.nt->OptionalHeader.Subsystem == 12) add_item("带有运行时服务的EFI驱动程序", 2, op_item);
		else if (pehead.nt->OptionalHeader.Subsystem == 13) add_item("EFI ROM镜像", 2, op_item);
		else if (pehead.nt->OptionalHeader.Subsystem == 14) add_item("Xbox系统", 2, op_item);
		else if (pehead.nt->OptionalHeader.Subsystem == 16) add_item("启动应用程序", 2, op_item);
		else add_item("错误的子系统", 2, op_item);
		auto dll_p = add_item("Dll属性", 2, op_item);
		if (pehead.nt->OptionalHeader.DllCharacteristics & 0x20) add_item("具有64位地址空间的ASLR; ", 0, dll_p);
		if (pehead.nt->OptionalHeader.DllCharacteristics & 0x40) add_item("DLL可以在加载时重新定位; ", 0, dll_p);
		if (pehead.nt->OptionalHeader.DllCharacteristics & 0x80) add_item("强制进行代码完整性检查; ", 0, dll_p);
		if (pehead.nt->OptionalHeader.DllCharacteristics & 0x100) add_item("该映像与数据执行预防(DEP)兼容; ", 0, dll_p);
		if (pehead.nt->OptionalHeader.DllCharacteristics & 0x200) add_item("映像可以被隔离，但不应该被隔离; ", 0, dll_p);
		if (pehead.nt->OptionalHeader.DllCharacteristics & 0x400) add_item("该映像不使用结构化异常处理(SEH)。在此映像中不能调用任何处理程序; ", 0, dll_p);
		if (pehead.nt->OptionalHeader.DllCharacteristics & 0x800) add_item("不要绑定映像; ", 0, dll_p);
		if (pehead.nt->OptionalHeader.DllCharacteristics & 0x1000) add_item("映像应该在AppContainer中执行; ", 0, dll_p);
		if (pehead.nt->OptionalHeader.DllCharacteristics & 0x2000) add_item("一个WDM驱动; ", 0, dll_p);
		if (pehead.nt->OptionalHeader.DllCharacteristics & 0x4000) add_item("映像支持控制流保护(Control Flow Guard); ", 0, dll_p);
		if (pehead.nt->OptionalHeader.DllCharacteristics & 0x8000) add_item("该映像是终端服务器感知的; ", 0, dll_p);
		if (!pehead.is_x32) {
			add_item(ldw_to_int("为堆栈保留的字节数:   ", pehead.nt64->OptionalHeader.SizeOfStackReserve).c_str(), 8, op_item);
			add_item(ldw_to_int("要提交给堆栈的字节数:   ", pehead.nt64->OptionalHeader.SizeOfStackCommit).c_str(), 8, op_item);
			add_item(ldw_to_int("为本地堆保留的字节数:   ", pehead.nt64->OptionalHeader.SizeOfHeapReserve).c_str(), 8, op_item);
			add_item(ldw_to_int("要为本地堆提交的字节数:   ", pehead.nt64->OptionalHeader.SizeOfHeapCommit).c_str(), 8, op_item);
			add_item(dw_to_int("该成员已过时:   ", pehead.nt64->OptionalHeader.LoaderFlags).c_str(), 4, op_item);
			add_item(dw_to_int("可选头的其余部分中的目录条目数:   ", pehead.nt64->OptionalHeader.NumberOfRvaAndSizes).c_str(), 4, op_item);
		}
		else {
			add_item(dw_to_int("为堆栈保留的字节数:   ", pehead.nt->OptionalHeader.SizeOfStackReserve).c_str(), 4, op_item);
			add_item(dw_to_int("要提交给堆栈的字节数:   ", pehead.nt->OptionalHeader.SizeOfStackCommit).c_str(), 4, op_item);
			add_item(dw_to_int("为本地堆保留的字节数:   ", pehead.nt->OptionalHeader.SizeOfHeapReserve).c_str(), 4, op_item);
			add_item(dw_to_int("要为本地堆提交的字节数:   ", pehead.nt->OptionalHeader.SizeOfHeapCommit).c_str(), 4, op_item);
			add_item(dw_to_int("该成员已过时:   ", pehead.nt->OptionalHeader.LoaderFlags).c_str(), 4, op_item);
			add_item(dw_to_int("可选头的其余部分中的目录条目数:   ", pehead.nt->OptionalHeader.NumberOfRvaAndSizes).c_str(), 4, op_item);
		}
		auto tab = add_item("表", 0, op_item);
		vector<string>tab_list{ "导出表","导入表","资源" ,"异常" ,"安全" ,"重定位表" ,"调试信息" ,"版权信息" ,"RVA OF GP" ,"TLS" ,
			"配置目录" ,"加载目录" ,"导入函数地址表(IAT)" ,"延迟导入表" ,"COM","空表" };
		auto ipt = !pehead.is_x32 ? pehead.nt64->OptionalHeader.DataDirectory : pehead.nt->OptionalHeader.DataDirectory;
		for (int i = 0; i < 16; i++) {
			auto out = add_item(tab_list[i].c_str(), 8, tab);
			stpt -= 8;
			add_item(dw_to_hex("RVA:   ", &ipt[i].VirtualAddress).c_str(), 4, out);
			add_item(dw_to_int("重定位块的大小:   ", ipt[i].Size).c_str(), 4, out);
		}
	}

	auto se_item = add_item("SECTION Header", 0);
	if (se_item) {
		auto sec_pt = pehead.section;
		for (int i = 0; i < pehead.nt->FileHeader.NumberOfSections;i++) {
			string name_;
			name_.append((char*)&sec_pt->Name, 8);
			auto _temp = add_item(s_to_fos("", name_).c_str(), 8, se_item);
			add_item(dw_to_hex("节的文件地址:   0x", &sec_pt->Misc.PhysicalAddress).c_str(), 4, _temp); 
			stpt -= 4;
			add_item(dw_to_int("节加载到内存时的总大小:   ", sec_pt->Misc.VirtualSize).c_str(), 4, _temp);
			add_item(dw_to_hex("节在内存中的偏移地址(VirtualAddress):   0x", &sec_pt->VirtualAddress).c_str(), 4, _temp);
			add_item(dw_to_int("节在文件中对齐后的尺寸:   ", sec_pt->SizeOfRawData).c_str(), 4, _temp);
			add_item(dw_to_hex("节区在文件中的偏移(PointerToRawData):   0x", &sec_pt->PointerToRawData).c_str(), 4, _temp);
			add_item(dw_to_hex(".obj文件有效:   0x", &sec_pt->PointerToRelocations).c_str(), 4, _temp);
			add_item(dw_to_hex("调试相关:   0x", &sec_pt->PointerToLinenumbers).c_str(), 4, _temp);
			add_item(w_to_int(".obj文件有效:   ", sec_pt->NumberOfRelocations).c_str(), 2, _temp);
			add_item(w_to_int("行号表中行号的数量:   ", sec_pt->NumberOfLinenumbers).c_str(), 2, _temp);
			auto ics = add_item("节的属性 ", 4, _temp);
			if (sec_pt->Characteristics & 0x00000008)add_item("该节不得填塞至下一边界线 ", 0, ics);
			if (sec_pt->Characteristics & 0x00000020)add_item("该节包含可执行代码 ", 0, ics);
			if (sec_pt->Characteristics & 0x00000040)add_item("该节包含初始化的数据 ", 0, ics);
			if (sec_pt->Characteristics & 0x00000080)add_item("该节包含未初始化的数据 ", 0, ics);
			if (sec_pt->Characteristics & 0x00000200)add_item("该节包含解释或其他信息 ", 0, ics);
			if (sec_pt->Characteristics & 0x00000800)add_item("该节将不会成为image的一部分 ", 0, ics);
			if (sec_pt->Characteristics & 0x00001000)add_item("该节包含COMDAT数据 ", 0, ics);
			if (sec_pt->Characteristics & 0x00004000)add_item("该节包含重置TLB项中的speculative异常处理位 ", 0, ics);
			if (sec_pt->Characteristics & 0x00008000)add_item("该节包含通过全局指针引用的数据 ", 0, ics);
			if (sec_pt->Characteristics & 0x00100000)add_item("在1字节的边界上对齐数据 ", 0, ics);
			if (sec_pt->Characteristics & 0x00200000)add_item("在2字节的边界上对齐数据 ", 0, ics);
			if (sec_pt->Characteristics & 0x00300000)add_item("在4字节的边界上对齐数据 ", 0, ics);
			if (sec_pt->Characteristics & 0x00400000)add_item("在8字节的边界上对齐数据 ", 0, ics);
			if (sec_pt->Characteristics & 0x00500000)add_item("在16字节的边界上对齐数据 ", 0, ics);
			if (sec_pt->Characteristics & 0x00600000)add_item("在32字节的边界上对齐数据 ", 0, ics);
			if (sec_pt->Characteristics & 0x00700000)add_item("在64字节的边界上对齐数据 ", 0, ics);
			if (sec_pt->Characteristics & 0x00800000)add_item("在128字节的边界上对齐数据 ", 0, ics);
			if (sec_pt->Characteristics & 0x00900000)add_item("在256字节的边界上对齐数据 ", 0, ics);
			if (sec_pt->Characteristics & 0x00A00000)add_item("在512字节的边界上对齐数据 ", 0, ics);
			if (sec_pt->Characteristics & 0x00B00000)add_item("在1024字节的边界上对齐数据 ", 0, ics);
			if (sec_pt->Characteristics & 0x00C00000)add_item("在2048字节的边界上对齐数据 ", 0, ics);
			if (sec_pt->Characteristics & 0x00D00000)add_item("在4096字节的边界上对齐数据 ", 0, ics);
			if (sec_pt->Characteristics & 0x00E00000)add_item("在8192字节的边界上对齐数据 ", 0, ics);
			if (sec_pt->Characteristics & 0x01000000)add_item("该节包含扩展的重新定位 ", 0, ics);
			if (sec_pt->Characteristics & 0x02000000)add_item("该节可以根据需要丢弃 ", 0, ics);
			if (sec_pt->Characteristics & 0x04000000)add_item("不能缓存该节 ", 0, ics);
			if (sec_pt->Characteristics & 0x08000000)add_item("该节不能分页 ", 0, ics);
			if (sec_pt->Characteristics & 0x10000000)add_item("该节可以在内存中共享 ", 0, ics);
			if (sec_pt->Characteristics & 0x20000000)add_item("该节可以作为代码执行 ", 0, ics);
			if (sec_pt->Characteristics & 0x40000000)add_item("该节可以读 ", 0, ics);
			if (sec_pt->Characteristics & 0x80000000)add_item("该节可以写 ", 0, ics);
			sec_pt++;
		}
	}

	if (pehead.imp) {
		auto t_p = pehead.imp;
		auto imp_item = add_item("IMPORT Table", 0);
		while (t_p->Characteristics) {
			auto name_foa_ = RvaToFoa(pehead.nt->OptionalHeader.ImageBase + t_p->Name);
			string dll_name_ = pehead.raw_val.data() + name_foa_;
			auto dll_imp = add_item(s_to_fos("", dll_name_).c_str(), 0, imp_item);
			auto int_item = add_item("INT", 0, dll_imp);
			auto iat_item = add_item("IAT", 0, dll_imp);

			auto ori_foa = RvaToFoa(pehead.nt->OptionalHeader.ImageBase + t_p->OriginalFirstThunk);
			DWORD* itd_ori = (DWORD*)(pehead.raw_val.data() + ori_foa);
			while (*itd_ori) {
				if (*itd_ori & 0x80000000) add_item(QString("%1").arg(((*itd_ori) << 1) >> 1), 0, int_item);
				else {
					ULONG var_ = *itd_ori + pehead.nt->OptionalHeader.ImageBase;
					auto foa_ = RvaToFoa(var_);
					_IMAGE_IMPORT_BY_NAME* itd = (_IMAGE_IMPORT_BY_NAME*)(pehead.raw_val.data() + foa_);
					string name_ = (char*)&itd->Name;
					add_item(s_to_fos(" ", name_).c_str(), 0, int_item);
				}
				itd_ori++;
			}

			auto fir_foa = RvaToFoa(pehead.nt->OptionalHeader.ImageBase + t_p->FirstThunk);
			DWORD* itd_fir = (DWORD*)(pehead.raw_val.data() + fir_foa);
			while (*itd_fir) {
				if (*itd_fir & 0x80000000) add_item(QString("%1").arg(((*itd_fir) << 1) >> 1), 0, iat_item);
				else {
					ULONG var_ = *itd_fir + pehead.nt->OptionalHeader.ImageBase;
					auto foa_ = RvaToFoa(var_);
					_IMAGE_IMPORT_BY_NAME* itd = (_IMAGE_IMPORT_BY_NAME*)(pehead.raw_val.data() + foa_);
					string name_ = (char*)&itd->Name;
					add_item(s_to_fos(" ", name_).c_str(), 0, iat_item);
				}
				itd_fir++;
			}
			t_p++;
		}
	}



	unsigned long long row_c = pehead.raw_val.length() / 16 + (pehead.raw_val.length() % 16 ? 1 : 0);
	ui->tableWidget->setRowCount(row_c);
	unsigned long row_c_ = 0;
	char cols = 0;
	string msg_val;
	for (auto i : pehead.raw_val) {
		if (cols == 17) {
			ui->tableWidget->setItem(row_c_, cols, new QTableWidgetItem(s_to_fos("", msg_val).c_str()));
			cols = 0;
			row_c_++;
			msg_val.clear();
		}
		if (!cols) {
			unsigned long len = row_c_ * 16;
			ui->tableWidget->setItem(row_c_, cols++, new QTableWidgetItem(dw_to_hex("", &len).c_str()));
		}
		msg_val.push_back(i), msg_val.push_back(' ');
		ui->tableWidget->setItem(row_c_, cols++, new QTableWidgetItem(c_to_hex(i).c_str()));
		QApplication::processEvents();
	}
}

ULONG PeFileAnalyzer::RvaToFoa(ULONG val) {
	ULONG rva = val - pehead.nt->OptionalHeader.ImageBase;
	ULONG PeEnd = pehead.dos->e_lfanew + sizeof(_IMAGE_NT_HEADERS);
	if (rva < PeEnd) return foa;

	for (ULONG i = 0; i < pehead.nt->FileHeader.NumberOfSections; i++) {
		UINT SizeInMemory = ceil((double)max((ULONG)pehead.section[i].Misc.VirtualSize, (ULONG)pehead.section[i].SizeOfRawData) / (double)pehead.nt->OptionalHeader.SectionAlignment) * pehead.nt->OptionalHeader.SectionAlignment;
		if (rva >= pehead.section[i].VirtualAddress && rva < (pehead.section[i].VirtualAddress + SizeInMemory)) {
			ULONG offset = rva - pehead.section[i].VirtualAddress;			//计算差值= RVA - 节.VirtualAddress
			return pehead.section[i].PointerToRawData + offset;				//FOA = 节.PointerToRawData + 差值
		}
	}
	return 0;
}

void PeFileAnalyzer::on_lineEdit_rva_textChanged(QString msg) {
	if (pehead.section == nullptr) {
		ui->label_rva->setText("空的节表!");
		return;
	}

	ULONG valfoa = 0;
	string str = msg.toStdString();
	if (str.size() % 2) str.insert(str.begin() + 1, '0');
	for (int i = 0; i < str.length();) {
		valfoa *= 16 * 16;
		uchar h = str[i++], t = str[i++];
		bool format = (h >= '0' && h <= '9') || (h >= 'a' && h <= 'f') || (h >= 'A' && h <= 'F');
		format &= (t >= '0' && t <= '9') || (t >= 'a' && t <= 'f') || (t >= 'A' && t <= 'F');
		if (!format) {
			ui->label_rva->setText("错误的16进制数值!");
			return;
		}
		auto t_p = h ? (h < 65 ? h - 48 : (h < 97 ? h - 55 : h - 87)) * 16 : 0;
		auto g_p = t ? (t < 65 ? t - 48 : (t < 97 ? t - 55 : t - 87)) : 0;
		valfoa = valfoa + t_p + g_p;
	}
	foa = RvaToFoa(valfoa);
	if (foa) {
		char buf[64];
		sprintf_s(buf, "FOA:0x%X", foa);
		ui->label_rva->setText(buf);
	}
	else ui->label_rva->setText("未找到匹配节!");
}

void PeFileAnalyzer::on_pushButton_foa_clicked() {
	if (!pehead.section || !foa)return;
	unsigned int rows = foa / 16;
	unsigned int cols = foa % 16 + 1;
	ui->tableWidget->setCurrentItem(ui->tableWidget->item(rows, cols));
}

void PeFileAnalyzer::on_treeWidget_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous){
	ui->tableWidget->clearSelection();
	string val = current->whatsThis(0).toStdString();
	unsigned int beg = atoi(val.c_str());
	unsigned int len = atoi(val.substr(val.find(',') + 1).c_str());
	unsigned int rows = beg / 16;
	unsigned int cols = beg % 16 + 1;
	while(len--){
		if (cols == 17)cols = 1, rows++;
		ui->tableWidget->setCurrentItem(ui->tableWidget->item(rows, cols++));
	}
}


void PeFileAnalyzer::dragEnterEvent(QDragEnterEvent* event){
	if (event->mimeData()->hasUrls()) event->acceptProposedAction();
	else event->ignore();
}
void PeFileAnalyzer::dropEvent(QDropEvent* event){
	QList<QUrl> urls = event->mimeData()->urls();
	if (urls.isEmpty()) return;
	file_name = urls[0].toLocalFile();
	ui->label_file->setText(file_name);

	if (ReadFile()) ShowValue();
}