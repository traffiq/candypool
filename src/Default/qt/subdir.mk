################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../qt/aboutdialog.cpp \
../qt/addressbookpage.cpp \
../qt/addresstablemodel.cpp \
../qt/askpassphrasedialog.cpp \
../qt/bitcoin.cpp \
../qt/bitcoinaddressvalidator.cpp \
../qt/bitcoinamountfield.cpp \
../qt/bitcoingui.cpp \
../qt/bitcoinstrings.cpp \
../qt/bitcoinunits.cpp \
../qt/clientmodel.cpp \
../qt/csvmodelwriter.cpp \
../qt/editaddressdialog.cpp \
../qt/guiutil.cpp \
../qt/monitoreddatamapper.cpp \
../qt/notificator.cpp \
../qt/optionsdialog.cpp \
../qt/optionsmodel.cpp \
../qt/overviewpage.cpp \
../qt/paymentserver.cpp \
../qt/qrcodedialog.cpp \
../qt/qvalidatedlineedit.cpp \
../qt/qvaluecombobox.cpp \
../qt/rpcconsole.cpp \
../qt/sendcoinsdialog.cpp \
../qt/sendcoinsentry.cpp \
../qt/signverifymessagedialog.cpp \
../qt/splashscreen.cpp \
../qt/transactiondesc.cpp \
../qt/transactiondescdialog.cpp \
../qt/transactionfilterproxy.cpp \
../qt/transactionrecord.cpp \
../qt/transactiontablemodel.cpp \
../qt/transactionview.cpp \
../qt/walletframe.cpp \
../qt/walletmodel.cpp \
../qt/walletstack.cpp \
../qt/walletview.cpp 

OBJS += \
./qt/aboutdialog.o \
./qt/addressbookpage.o \
./qt/addresstablemodel.o \
./qt/askpassphrasedialog.o \
./qt/bitcoin.o \
./qt/bitcoinaddressvalidator.o \
./qt/bitcoinamountfield.o \
./qt/bitcoingui.o \
./qt/bitcoinstrings.o \
./qt/bitcoinunits.o \
./qt/clientmodel.o \
./qt/csvmodelwriter.o \
./qt/editaddressdialog.o \
./qt/guiutil.o \
./qt/monitoreddatamapper.o \
./qt/notificator.o \
./qt/optionsdialog.o \
./qt/optionsmodel.o \
./qt/overviewpage.o \
./qt/paymentserver.o \
./qt/qrcodedialog.o \
./qt/qvalidatedlineedit.o \
./qt/qvaluecombobox.o \
./qt/rpcconsole.o \
./qt/sendcoinsdialog.o \
./qt/sendcoinsentry.o \
./qt/signverifymessagedialog.o \
./qt/splashscreen.o \
./qt/transactiondesc.o \
./qt/transactiondescdialog.o \
./qt/transactionfilterproxy.o \
./qt/transactionrecord.o \
./qt/transactiontablemodel.o \
./qt/transactionview.o \
./qt/walletframe.o \
./qt/walletmodel.o \
./qt/walletstack.o \
./qt/walletview.o 

CPP_DEPS += \
./qt/aboutdialog.d \
./qt/addressbookpage.d \
./qt/addresstablemodel.d \
./qt/askpassphrasedialog.d \
./qt/bitcoin.d \
./qt/bitcoinaddressvalidator.d \
./qt/bitcoinamountfield.d \
./qt/bitcoingui.d \
./qt/bitcoinstrings.d \
./qt/bitcoinunits.d \
./qt/clientmodel.d \
./qt/csvmodelwriter.d \
./qt/editaddressdialog.d \
./qt/guiutil.d \
./qt/monitoreddatamapper.d \
./qt/notificator.d \
./qt/optionsdialog.d \
./qt/optionsmodel.d \
./qt/overviewpage.d \
./qt/paymentserver.d \
./qt/qrcodedialog.d \
./qt/qvalidatedlineedit.d \
./qt/qvaluecombobox.d \
./qt/rpcconsole.d \
./qt/sendcoinsdialog.d \
./qt/sendcoinsentry.d \
./qt/signverifymessagedialog.d \
./qt/splashscreen.d \
./qt/transactiondesc.d \
./qt/transactiondescdialog.d \
./qt/transactionfilterproxy.d \
./qt/transactionrecord.d \
./qt/transactiontablemodel.d \
./qt/transactionview.d \
./qt/walletframe.d \
./qt/walletmodel.d \
./qt/walletstack.d \
./qt/walletview.d 


# Each subdirectory must supply rules for building sources it contributes
qt/%.o: ../qt/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


