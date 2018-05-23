#include "mainwindow.h"

#include <iostream>
#include <gtkmm.h>

#include "common/core.h"
#include "widgets/resourceview.h"
#include "widgets/processview.h"

namespace ProcessMonitor {

MainWindow* MainWindow::Create() {
  auto builder = Gtk::Builder::create_from_resource("/windows/mainwindow.glade");
  ProcessMonitor::MainWindow *window;
  builder->get_widget_derived("mainwindow", window);

  return window;
}

MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade) :
  Gtk::Window(cobject),
  builder_(refGlade),
  timer_(0)
{
  // init MainWindow data
  builder_->get_widget("radioprocess", radioprocess_);
  builder_->get_widget("radioresources", radioresources_);
  builder_->get_widget("notebookmain", notebookmain_);

  // fill up notebook
  processview_ = ProcessView::Create();
  notebookmain_->append_page(*processview_);

  resourceview_ = ResourceView::Create();
  notebookmain_->append_page(*resourceview_);

  show_all();

  // connect 
  sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &MainWindow::on_timeout), 0);
  Glib::signal_timeout().connect(my_slot, 40);

  // connect events
  radioprocess_->signal_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::on_tabbutton1_activate)
  );
  radioresources_->signal_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::on_tabbutton2_activate)
  );
}

bool MainWindow::on_timeout(int timer_number)
{
  if (timer_ == 0)
  {
    // update core
    Core::getInstance().refresh();
    resourceview_->fetch();
    processview_->update();

    timer_ = 25;
  }
  timer_--;

  resourceview_->update(40);

  return true;
}

void MainWindow::on_tabbutton1_activate()
{
  notebookmain_->set_current_page(0);
}

void MainWindow::on_tabbutton2_activate()
{
  notebookmain_->set_current_page(1);
}

}
